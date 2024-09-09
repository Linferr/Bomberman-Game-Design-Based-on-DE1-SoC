import time
import random
from threading import Lock, Timer

#TODO reset position between games

starting_positions = [(0,0), (14, 14), (0, 14), (14, 0)]

class Session():
    def __init__(self, session_id):
        self.session_id = session_id
        self.state = 'empty'
        self.waiting_send = False #False - sw; True - sf;
        self.max_players = 4

        self.users = {}
        self.user_count = 0
        self.user_slots = [i for i in range(0, self.max_players)]
        self.host = 0
        self.rank_reversed = []
        self.user_lk = Lock()

        self.bombs = {}
        self.bomb_counter = 0
        self.bomb_lk = Lock()

        self.explosions = []
        #self.explosions_lk = Lock()

        self.game_players_left = 0
    
    def __str__(self):
        return f"Session:\nsession_id: {self.session_id}"

    def entity_list(self):
        #print(f"##### ##### ##### SESSION CALLED: {time.monotonic_ns()}")
        render_string = '['

        #state indicator
        if self.state == 'active':
            #print(f"##### ##### ##### START ACTIVE RENDER STRING: {time.monotonic_ns()}")
            render_string += 'sa;'

            #positions
            #self.user_lk.acquire()
            render_string += 'p'
            for user_id in range(0, self.max_players):
                if user_id in self.users.keys():
                    try:
                        info = self.users[user_id]['entity']
                        render_string += f"{info['position_x']:x}{info['position_y']:x}"
                    except KeyError:
                        render_string += 'ff'
                else:
                    render_string += 'ff'
            render_string += ';'

            #print(f"##### ##### ##### USER DONE: {time.monotonic_ns()}")

            #health
            render_string += 'h'
            for user_id in range(0, self.max_players):
                if user_id in self.users.keys():
                    try:
                        health = self.users[user_id]['health']
                    except KeyError:
                        health = 0
                    if health > 0:
                        render_string += f"{health}"
                    else:
                        render_string += '0'
                else:
                    render_string += '0'
            render_string += ';'
            #self.user_lk.release()

            #print(f"##### ##### ##### HEALTH DONE: {time.monotonic_ns()}")

            #bombs
            #self.bomb_lk.acquire()
            if len(self.bombs.values()) > 0:
                render_string += 'b'
                for bomb in self.bombs.values():
                    render_string += f"{bomb['entity-type'] + 4}{bomb['position_x']:x}{bomb['position_y']:x}"
                render_string += ';'
            #self.bomb_lk.release()
            
            #print(f"##### ##### ##### BOMBS DONE: {time.monotonic_ns()}")

            #explosions
            #self.explosions_lk.acquire()
            if len(self.explosions) > 0:
                render_string += 'e'
                while (len(self.explosions) > 0):
                    render_string += self.explosions.pop()
                render_string += ';'
            #self.explosions_lk.release()
            #print(f"##### ##### ##### EXPLOSIONS DONE: {time.monotonic_ns()}")
        else:
            if self.waiting_send and len(self.rank_reversed) != 0:
                #health
                render_string += 'h'
                for user_id in range(0, self.max_players):
                    if user_id in self.users.keys():
                        health = self.users[user_id]['health']
                        if health > 0:
                            render_string += f"{health}"
                        else:
                            render_string += '0'
                    else:
                        render_string += '0'
                render_string += ';'

                #rankings
                rankstring = ''
                for i in range(len(self.rank_reversed) - 1, -1, -1):
                    rankstring += str(self.rank_reversed[i] + 1)
                render_string += f'sf{rankstring};'
            else:
                #wait screen
                render_string += 'sw;'
        

        render_string += ']'
        #print(f"##### ##### ##### SESSION RETURNING: {time.monotonic_ns()}")
        return render_string
    
    def game_state(self):
        #'empty', 'waiting', or 'active'
        return self.state

    def join_user(self):
        if (self.user_count >= self.max_players):
            return -1

        self.user_lk.acquire()
        user_id = self.user_slots.pop(0)

        self.users[user_id] = {
            'entity':{
                'id':user_id, 
                'entity_type':f'player-{user_id}', 
                'position_x':starting_positions[user_id][0], 
                'position_y':starting_positions[user_id][1]
            },
            'health':3,
            'ranking':0
        }
        
        self.user_count += 1
        if (self.user_count == 1):
            self.host = user_id
            self.state = 'waiting'

        self.user_lk.release()

        return user_id
    
    def leave_user(self, user_id):
        if user_id in self.users.keys():
            self.user_lk.acquire()
            self.users.pop(user_id)
            self.user_slots.append(user_id)
            self.user_slots.sort()

            self.user_count -= 1
            if self.user_count > 0 and self.host == user_id:
                self.host = list(self.users.keys())[0]
            elif self.user_count == 0:
                self.state = 'empty'

            self.user_lk.release()
            return True
        else:
            return False
    
    def info(self, user_id):
        return {'session_id':self.session_id,
                'current_count':self.user_count,
                'max_count':self.max_players,
                'host':(user_id == self.host),
                'state':self.state}
    
    def start(self, user_id):
        if (user_id == self.host) and (self.user_count > 1):
            #self.user_lk.acquire()
            #self.bomb_lk.acquire()
            #self.explosions_lk.acquire()

            self.game_players_left = self.user_count
            self.rank_reversed = []

            for i in range (0, self.user_count):
                self.users[i] = {
                    'entity':{
                        'id':i, 
                        'entity_type':f'player-{i}', 
                        'position_x':starting_positions[i][0], 
                        'position_y':starting_positions[i][1]
                    },
                    'health':3,
                    'ranking':0
                }

            self.bombs = {}

            self.explosions = []

            #self.explosions_lk.release()
            #self.bomb_lk.release()
            #self.user_lk.release()

            self.state = 'active'
            print(f"SESSION {self.session_id}: Game started with {self.user_count} players")
            return True
        else:
            return False

    # gameplay
    def movement_check(self, user_id, direction, xpos, ypos):
        #user_id - user to move
        #direction - 'left', 'right', 'up', and 'down'
        #xpos, ypos = (x, y) current location
        
        #intended movement location
        if direction == 'left':
            newx = xpos - 1
            newy = ypos
        elif direction == 'right':
            newx = xpos + 1
            newy = ypos
        elif direction == 'up':
            newx = xpos
            newy = ypos - 1
        elif direction == 'down':
            newx = xpos
            newy = ypos + 1
        else:
            newx = xpos
            newy = ypos
        
        #bounds check
        if (newx < 0 or newx > 14 or newy < 0 or newy > 14):
            return None

        #collision check
        for user in self.users.values():
            if newx == user['entity']['position_x'] and newy == user['entity']['position_y']:
                return None
        
        return (newx, newy)

    def movement(self, user_id, direction):
        #left, right, up, down
        if self.state != 'active':
            return None

        if user_id not in self.users.keys():
            return None

        #self.user_lk.acquire()
        user = self.users[user_id]
        xpos = user['entity']['position_x']
        ypos = user['entity']['position_y']

        check = self.movement_check(user_id, direction, xpos, ypos)

        if check != None:
            user['entity']['position_x'] = check[0]
            user['entity']['position_y'] = check[1]

        print(f"SESSION {self.session_id}: Player {user_id} moved to {user['entity']['position_x']}, {user['entity']['position_y']}")
        #self.user_lk.release()
        return None
    
    def place(self, user_id, entity_type):
        if self.state != 'active':
            return None
        if user_id not in self.users.keys():
            return None

        #grab position
        #self.user_lk.acquire()
        x = self.users[user_id]['entity']['position_x']
        y = self.users[user_id]['entity']['position_y']

        self.bomb_lk.acquire()
        newbomb_id = self.bomb_counter
        self.bomb_counter += 1
        self.bomb_lk.release()

        newbomb = {
            'id':newbomb_id,
            'entity-type':entity_type,
            'position_x':x,
            'position_y':y
        }

        if entity_type == 0:
            #test bomb
            newbomb['entity-type'] = 0
            t = Timer(5, self.detonate_test, args=(newbomb_id,))
            t.start()
        elif entity_type == 1:
            #circular bomb
            newbomb['entity-type'] = 1
            t = Timer(2, self.detonate_3by3, args=(newbomb_id,(x, y)))
            t.start()
        elif entity_type == 2:
            #horizontal line bomb
            newbomb['entity-type'] = 2
            t = Timer(2, self.detonate_line, args=(newbomb_id,(x, y),False))
            t.start()
        elif entity_type == 3:
            #vertical line bomb
            newbomb['entity-type'] = 3
            t = Timer(2, self.detonate_line, args=(newbomb_id,(x, y),True))
            t.start()
        else:
            #self.bomb_lk.release()
            #self.user_lk.release()
            return None

        print(f"SESSION {self.session_id}: Player {user_id} placed entity#{entity_type} at {x}, {y}")
        self.bombs[newbomb_id] = newbomb
        #self.bomb_lk.release()
        #self.user_lk.release()


    def char_status(self, user_id):
        if user_id not in self.users.keys():
            return None

        #self.user_lk.acquire()
        result = {
            'id':user_id,
            'health':self.users[user_id]['health'],
            'ranking':self.users[user_id]['ranking']
        }
        #self.user_lk.release()

        return result
    
    def damage(self, user_id, amount):
        #damage a user by a specific amount, should only be called on alive characters
        user = self.users[user_id]

        if user['entity']['position_x'] == 15 and user['entity']['position_y'] == 15:
            #already dead
            return

        user['health'] -= amount
        if user['health'] <= 0:
            user['entity']['position_x'] = 15
            user['entity']['position_y'] = 15
            user['health'] = 0
            user['ranking'] = self.game_players_left
            self.game_players_left -= 1
            print(f"SESSION {self.session_id}: Player {user_id} is out, rank# {user['ranking']}")
            self.rank_reversed.append(user_id)

            if self.game_players_left == 1:
                self.end_game()
        
        print(f"SESSION {self.session_id}: Player {user_id} took {amount} damage")
    
    def distance(self, user_id, x, y):
        try:
            user_x = self.users[user_id]['entity']['position_x']
            user_y = self.users[user_id]['entity']['position_y']
            return ((x - user_x) ** 2 + (y - user_y) ** 2) ** 0.5
        except KeyError:
            return 100

    def revert(self):
        self.waiting_send = False
    
    def end_game(self):
        winner = None

        for user_id in self.users.keys():
            if self.users[user_id]['health'] > 0:
                winner = user_id
        
        if winner != None:
            self.users[winner]['ranking'] = 1
            self.rank_reversed.append(winner)
            self.state = 'waiting'
            self.waiting_send = True
            t = Timer(15, self.revert, args=())
            t.start()
            print(f"SESSION {self.session_id}: Player {winner} won")
            print(f"SESSION {self.session_id}: {self.rank_reversed}")

    def detonate_test(self, bomb_id):
        #bomb that sweeps the playing field, killing all players except one
        if self.state != 'active':
            return False
        
        survivor = random.randint(0, self.user_count - 1)

        #self.user_lk.acquire()
        #hit detection
        for i in range(0, self.user_count):
            if (i != survivor):
                self.damage(i, 0) #NO EFFECT CURRENTLY
        #self.user_lk.release()

        #bomb removal
        #self.bomb_lk.acquire()
        self.bombs.pop(bomb_id)
        #self.bomb_lk.release()

        return True

    def detonate_circular(self, bomb_id, center, radius):
        #bomb_id - id of bomb that spawned this func
        #center - (x, y) position
        #radius - of explosion

        if self.state != 'active':
            return False 
        
        #hit registration
        #self.user_lk.acquire()
        for i in range(0, self.user_count):
            if self.distance(i, center[0], center[1]) < radius:
                self.damage(i, 1)
        #self.user_lk.release()

        #bomb removal
        #self.bomb_lk.acquire()
        self.bombs.pop(bomb_id)
        #self.bomb_lk.release()

        return True

    def detonate_3by3(self, bomb_id, center):
        if self.state != 'active':
            return False 
        
        #self.user_lk.acquire()
        #hit registration
        for i in self.users.keys():
            print(f"Grid detonation {bomb_id} - Checking {i}")
            if self.distance(i, center[0], center[1]) < 1.5:
                print(f"{i} is within distance")
                self.damage(i, 1)
        #self.user_lk.release()
        
        #spawn explosions
        #self.explosions_lk.acquire()
        self.explosions.append(f"5{center[0]:x}{center[1]:x}")
        #self.explosions_lk.release()
        
        #bomb removal
        #self.bomb_lk.acquire()
        self.bombs.pop(bomb_id)
        #self.bomb_lk.release()

        return True
    
    def detonate_line(self, bomb_id, center, orientation):
        #bomb_id - id of bomb that spawned this func
        #center - (x, y) position
        #orientation - True=horizontal, False=vertical

        if self.state != 'active':
            return False
        
        #hit registration
        for user_id in self.users.keys():
            print(f"Line detonation {bomb_id}: checking {user_id}")
            if orientation:
                #horizontal
                #self.user_lk.acquire()
                try:
                    user_y = self.users[user_id]['entity']['position_y']
                except KeyError:
                    user_y = 15

                if (user_y == center[1]):
                    self.damage(user_id, 1)

                #self.user_lk.release()

            else:
                #vertical
                #self.user_lk.acquire()
                try:
                    user_x = self.users[user_id]['entity']['position_x']
                except KeyError:
                    user_x = 15

                if (user_x == center[0]):
                    self.damage(user_id, 1)

                #self.user_lk.release()
        
        #spawn explosions
        #self.explosions_lk.acquire()
        if orientation:
            #horizontal
            self.explosions.append(f"7{center[0]:x}{center[1]:x}")
        else:
            #vertical
            self.explosions.append(f"6{center[0]:x}{center[1]:x}")
        #self.explosions_lk.release()
        
        #bomb removal
        #self.bomb_lk.acquire()
        self.bombs.pop(bomb_id)
        #self.bomb_lk.release()

        return True
        