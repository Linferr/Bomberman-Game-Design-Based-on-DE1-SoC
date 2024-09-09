import random
from threading import Lock, Timer

#TODO reset position between games

starting_positions = [(0,0), (14, 14), (0, 14), (14, 0)]

class Session():
    def __init__(self, session_id):
        self.session_id = session_id
        self.state = 'empty'
        self.max_players = 4

        self.players = {}
        self.player_count = 0
        self.player_slots = [i for i in range(0, self.max_players)]
        self.host = 0
        self.player_lk = Lock()

        self.bombs = {}
        self.bomb_counter = 0
        self.bomb_lk = Lock()

        self.game_players_left = 0
    
    def __str__(self):
        return f"Session:\nsession_id: {self.session_id}"

    def entity_list(self):
        ret = []
        for player in self.players.values():
            if (player['health'] > 0):
                ret.append(player['entity'])
        return ret + list(self.bombs.values())
    
    def game_state(self):
        #'empty', 'waiting', or 'active'
        return self.state

    def join_player(self):
        if (self.player_count >= self.max_players):
            return -1

        self.player_lk.acquire()
        player_id = self.player_slots.pop(0)

        self.players[player_id] = {
            'entity':{
                'id':player_id, 
                'entity_type':f'player-{player_id}', 
                'position_x':starting_positions[player_id][0], 
                'position_y':starting_positions[player_id][1]
            },
            'health':3,
            'ranking':0
        }
        
        self.player_count += 1
        if (self.player_count == 1):
            self.host = player_id
            self.state = 'waiting'

        self.player_lk.release()

        return player_id
    
    def leave_player(self, player_id):
        if player_id in self.players.keys():
            self.player_lk.acquire()
            self.players.pop(player_id)
            self.player_slots.append(player_id)
            self.player_slots.sort()

            self.player_count -= 1
            if self.player_count > 0 and self.host == player_id:
                self.host = list(self.players.keys())[0]
            elif self.player_count == 0:
                self.state = 'empty'

            self.player_lk.release()
            return True
        else:
            return False
    
    def info(self, player_id):
        return {'session_id':self.session_id, 'current_count':self.player_count, 'max_count':self.max_players, 'host':(player_id == self.host)}
    
    def start(self, player_id):
        if (player_id == self.host) and (self.player_count > 1):
            self.state = 'active'
            self.game_players_left = self.player_count

            return True
        else:
            return False

    # gameplay
    def movement(self, player_id, direction):
        #1 - left, 2 - right, 3 - up, 4 - down
        if self.state != 'active':
            return None

        if player_id not in self.players.keys():
            return None

        self.player_lk.acquire()
        player = self.players[player_id]

        if direction == 1:
            #left
            if not player['entity']['position_x'] == 0:
                player['entity']['position_x'] -= 1
        elif direction == 2:
            #right
            if not player['entity']['position_x'] == 14:
                player['entity']['position_x'] += 1
        elif direction == 3:
            #up
            if not player['entity']['position_y'] == 14:
                player['entity']['position_y'] += 1
        elif direction == 4:
            #down
            if not player['entity']['position_y'] == 0:
                player['entity']['position_y'] -= 1
        print(f"SESSION {self.session_id}: Player {player_id} moved to {player['entity']['position_x']}, {player['entity']['position_y']}")
        self.player_lk.release()
        return None
    
    def place(self, player_id, entity_type):
        if self.state != 'active':
            return None
        if player_id not in self.players.keys():
            return None

        #grab position
        self.player_lk.acquire()
        x = self.players[player_id]['entity']['position_x']
        y = self.players[player_id]['entity']['position_y']

        self.bomb_lk.acquire()
        newbomb = {
            'id':self.bomb_counter,
            'entity-type':entity_type,
            'position_x':x,
            'position_y':y
        }

        if entity_type == 0:
            #test bomb
            newbomb['entity-type'] = 'bomb-0'
            t = Timer(5, self.detonate_test, args=(self.bomb_counter,))
            t.start()
        else:
            self.bomb_lk.release()
            self.player_lk.release()
            return None

        print(f"SESSION {self.session_id}: Player {player_id} placed entity#{entity_type} at {x}, {y}")
        self.bombs[self.bomb_counter] = newbomb
        self.bomb_counter += 1
        self.bomb_lk.release()
        self.player_lk.release()


    def char_status(self, player_id):
        if player_id not in self.players.keys():
            return None

        self.player_lk.acquire()
        result = {
            'health':self.players[player_id]['health'],
            'ranking':self.players[player_id]['ranking']
        }
        self.player_lk.release()

        return result
    
    def damage(self, player_id, amount):
        #damage a player by a specific amount
        self.player_lk.acquire()
        player = self.players[player_id]

        player['health'] -= amount
        if player['health'] <= 0:
            player['health'] = 0
            player['ranking'] = self.game_players_left
            self.game_players_left -= 1
            print(f"SESSION {self.session_id}: Player {player_id} is out, rank# {player['ranking']}")

            self.player_lk.release()

            if self.game_players_left == 1:
                self.end_game()
    
    def end_game(self):
        winner = None

        for player_id in self.players.keys():
            if self.players[player_id]['health'] > 0:
                winner = player_id
        
        if winner != None:
            self.players[winner]['ranking'] = 1
            self.state = 'waiting'
            print(f"SESSION {self.session_id}: Player {winner} won")

    def detonate_test(self, bomb_id):
        #bomb that sweeps the playing field, killing all players except one
        if self.state != 'active':
            return False
        
        survivor = random.randint(0, self.player_count - 1)

        #hit detection
        for i in range(0, self.player_count):
            if (i != survivor):
                self.damage(i, 20)

        #bomb removal
        self.bomb_lk.acquire()
        self.bombs.pop(bomb_id)
        self.bomb_lk.release()

        return True

    def detonate_circular(self, bomb_id, center, radius):
        pass