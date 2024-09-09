import threading

from session import Session

class Multiplexer():
    def __init__(self, name, max_sessions=4, max_players=10):
        self.name = name
        self.max_sessions = max_sessions
        self.max_players = max_players

        self.session_table = {}
        self.session_lk = threading.Lock()

        self.user_counter = (self.max_sessions // 100) + 100
        self.uid_sid_table = {}
        self.user_lk = threading.Lock()

        #initialize sessions
        for sid in range(0, max_sessions):
            self.session_table[sid] = None
    
    def __str__(self):
        return f"Multiplexer {self.name}\nSessions: {str(self.session_table)}\nUsers: {str(self.uid_sid_table)}"

    def session_exists(self, session_id):
        """
        Return Value:
        - boolean indicating if the given session_id exists
        """

        return session_id in self.session_table.keys() and self.session_table[session_id] != None
    
    def registration_session_create(self):
        """
        Creates a new session.

        Return Value:
        - new session id, or -1 if error.
        """

        self.session_lk.acquire()

        sid = -1
        for check in range(0, len(self.session_table)):
            if self.session_table[check] == None:
                self.session_table[check] = {
                    'object': Session(check),
                    'id_table': {},
                    'host': -1,
                    'lobby': []
                }
                sid = check

        self.session_lk.release()

        return sid
    
    def registration_session_destroy(self, session_id):
        """
        Destroys an existing session.

        Return Value:
        - boolean indicating operation success
        """

        if self.session_exists(session_id):
            #destroy session
            self.session_lk.acquire()
            self.session_table[session_id] = None
            self.session_lk.release()

            #destroy uid table translations
            self.user_lk.acquire()
            for uid in self.uid_sid_table.keys():
                if self.uid_sid_table[uid] == session_id:
                    self.uid_sid_table.pop(uid)
            self.user_lk.release()
            return True
        
        return False
    
    def session_list(self):
        """
        Return a list of existing session ids.

        Return Value:
        - list of session ids
        """

        return list(filter(lambda sid : (self.session_table[sid] != None), self.session_table.keys()))
    
    def registration_session_render(self, session_id):
        """
        Return a list of the entities inside a specific session

        Return Value:
        - list of entities, None if session_id is invalid
        """

        if self.session_exists(session_id):
            return self.session_table.get(session_id).get('object').entity_list()
        else:
            return None
    
    def registration_session_state(self, session_id):
        """
        Returns the current status of the session

        Return Value:
        - session status: 'empty', 'waiting' (to start), 'active', None if session_id is invalid
        """

        if self.session_exists(session_id):
            return self.session_table.get(session_id).get('object').game_state()
        else:
            return None
    
    def session_join(self, session_id):
        """
        Attempts to add user to specified session (session_id)

        Return Value:
        - user_id for that session, '400' if session could not be joined, '404' if session not found
        """

        if self.session_exists(session_id):
            #create a new user_id for this user
            self.user_lk.acquire()
            user_id = self.user_counter
            self.user_counter += 1
            self.user_lk.release()

            #attempt to join the user into the session
            self.session_lk.acquire()

            session_dict = self.session_table.get(session_id)

            #check for player capacity
            if len(session_dict['lobby']) == self.max_players:
                self.session_lk.release()
                return '400'
            
            session_dict['lobby'].append(user_id)

            if session_dict['host'] == -1:
                session_dict['host'] = user_id

            self.session_lk.release()

            #add uid -> sid translation in table
            self.user_lk.acquire()
            self.uid_sid_table[user_id] = session_id
            self.user_lk.release()

            return user_id
        else:
            return '404'
            
    def session_leave(self, session_id, user_id):
        """
        Attempts to remove user from specified session

        Return Value:
        - boolean indicating operation success
        """

        if self.session_exists(session_id):
            success = self.session_table.get(session_id).leave_user(user_id)
            return success
        return False
    
    def session_info(self, session_id, user_id):
        """
        Return Value:
        - dict info of specified session, None if error
        """
        if self.session_exists(session_id):
            return self.session_table.get(session_id).info(user_id)
        return None
    
    def session_start(self, session_id, user_id):
        """
        As the specified user, attempt to start the specified session.

        Return Value:
        - boolean indicating operation success
        """
        if self.session_exists(session_id):
            return self.session_table.get(session_id).start(user_id)
        return False
    
    def gameplay_move(self, session_id, user_id, direction):
        """
        User movement function.
        """
        if self.session_exists(session_id):
            self.session_table.get(session_id).movement(user_id, direction)
    
    def gameplay_place(self, session_id, user_id, entity_type):
        """
        User placement function.
        """
        if self.session_exists(session_id):
            self.session_table.get(session_id).place(user_id, entity_type)
    
    def gameplay_status(self, session_id, user_id):
        """
        User character status function.

        Return Value:
        - dict containing character status, or None if error.
        """
        if self.session_exists(session_id):
            return self.session_table.get(session_id).char_status(user_id)
        return None