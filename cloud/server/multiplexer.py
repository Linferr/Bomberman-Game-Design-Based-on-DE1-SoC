import time
import threading
import uuid
import sqlite3 as sl

if __name__ == "server.multiplexer":
    from server.session import Session
else:
    from session import Session

class Multiplexer():
    def __init__(self, name):
        self.name = name

        self.counter = 1
        self.sessions = {}
        self.sessions_lk = threading.Lock()

        self.users = {}
        self.users_lk = threading.Lock()

        self.translations = {}
        self.translations_lk = threading.Lock()

        self.general = 0
        self.general_lk = threading.Lock()
    
    def __str__(self):
        return f"Multiplexer {self.name}\nSessions: {str(self.sessions)}"

    def translate(self, gen_id):
        #print(f"translate searching for {gen_id} in {self.translations}")
        self.translations_lk.acquire()
        ret = self.translations.get(gen_id, (None, None))
        self.translations_lk.release()
        return ret[0], ret[1]
    
    def registration_session_create(self):
        """
        Creates a new session.

        Return Value:
        - new session id, or -1 if error.
        """

        self.sessions_lk.acquire()

        session_id = self.counter
        self.counter += 1

        self.sessions[session_id] = Session(session_id)

        self.sessions_lk.release()

        return session_id
    
    def registration_session_destroy(self, session_id):
        """
        Destroys an existing session.

        Return Value:
        - boolean indicating operation success
        """

        if session_id in self.sessions.keys():
            self.sessions_lk.acquire()
            self.sessions.pop(session_id)
            self.sessions_lk.release()
            return True
        return False

    def session_list(self):
        """
        Return a list of existing session ids.

        Return Value:
        - list of session ids
        """

        return list(self.sessions.keys())
    
    def registration_session_render(self, session_id):
        """
        Return a list of the entities inside a specific session

        Return Value:
        - list of entities, None if session_id is invalid
        """
        #print(f"##### ##### MUX CALLED: {time.monotonic_ns()}")

        if session_id in self.sessions.keys():
            return self.sessions.get(session_id).entity_list()
        else:
            return None
    
    def registration_session_state(self, session_id):
        """
        Returns the current status of the session

        Return Value:
        - session status: 'empty', 'waiting' (to start), 'active', None if session_id is invalid
        """

        if session_id in self.sessions.keys():
            return self.sessions.get(session_id).game_state()
        else:
            return None
    
    def user_create(self, username, saltedhash):
        """
        Create a new user account.
        """
        self.users_lk.acquire()
        #self.users[username] = saltedhash
        con = sl.connect('users.db')
        sql = 'INSERT INTO user (name, salted) VALUES (?, ?)'
        data = [(username, saltedhash)]
        with con:
            con.executemany(sql, data)
        self.users_lk.release()

    def user_login(self, username, saltedhash):
        """
        Attempt to login with the given credentials

        Return Value:
        - id for user, None if unsuccessful
        """
        con = sl.connect('users.db')
        with con:
            data = con.execute(f'SELECT * FROM user WHERE name = \'{username}\'')
        
        for row in data:
            if row[0] == username and row[1] == saltedhash:
        # if username in self.users.keys():
        #     if self.users[username] == saltedhash:
                #create entry in translations table
                ret = uuid.uuid4()
                self.translations_lk.acquire()
                self.translations[str(ret)] = (None, None)
                self.translations_lk.release()
                return str(ret)
        return None
    
    def user_logout(self, gen_id):
        """
        Logs out the user with the given gen_id
        """
        self.translations_lk.acquire()
        self.translations.pop(gen_id, None)
        self.translations_lk.release()
    
    def session_join(self, gen_id, session_id):
        """
        Attempts to add user to specified session (session_id)

        Return Value:
        - 200 if success, 400 if session could not be joined, 404 if session not found
        """
        if gen_id not in self.translations.keys():
            return 400
        
        if self.translations[gen_id] != (None, None):
            return 400 #already in a session

        if session_id in self.sessions.keys():
            user_id = self.sessions.get(session_id).join_user()
            if user_id == -1:
                return 400
            else:
                self.translations_lk.acquire()
                self.translations[gen_id] = (session_id, user_id)
                self.translations_lk.release()
                return 200
        else:
            return 404
            
    def session_leave(self, gen_id):
        """
        Attempts to remove user from specified session

        Return Value:
        - boolean indicating operation success
        """
        session_id, user_id = self.translate(gen_id)

        self.translations_lk.acquire()
        self.translations[gen_id] = (None, None)
        self.translations_lk.release()

        if session_id in self.sessions.keys():
            success = self.sessions.get(session_id).leave_user(user_id)
            return success
        return False
    
    def session_info(self, gen_id):
        """
        Return Value:
        - dict info of specified session, None if error
        """
        session_id, user_id = self.translate(gen_id)

        if session_id in self.sessions.keys():
            return self.sessions.get(session_id).info(user_id)
        return None
    
    def session_start(self, gen_id):
        """
        As the specified user, attempt to start the specified session.

        Return Value:
        - boolean indicating operation success
        """
        session_id, user_id = self.translate(gen_id)

        if session_id in self.sessions.keys():
            return self.sessions.get(session_id).start(user_id)
        return False
    
    def gameplay_move(self, gen_id, direction):
        """
        User movement function.
        """
        session_id, user_id = self.translate(gen_id)

        if session_id in self.sessions.keys():
            self.sessions.get(session_id).movement(user_id, direction)
    
    def gameplay_place(self, gen_id, entity_type):
        """
        User placement function.
        """
        session_id, user_id = self.translate(gen_id)

        if session_id in self.sessions.keys():
            self.sessions.get(session_id).place(user_id, entity_type)
    
    def gameplay_status(self, gen_id):
        """
        User character status function.

        Return Value:
        - dict containing character status, or None if error.
        """
        session_id, user_id = self.translate(gen_id)

        if session_id in self.sessions.keys():
            return self.sessions.get(session_id).char_status(user_id)
        return None