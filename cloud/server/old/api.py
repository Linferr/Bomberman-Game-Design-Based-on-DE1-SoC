import json
import connexion
from multiplexer import Multiplexer

multiplexer = Multiplexer("cnxn_mux")

def extract_cookie(cookies, name):
    if name not in cookies:
        return None

    value = cookies[name]

    if len(value) == 0:
        return None
    
    return int(value)

#API FUNCTIONS

# Registration

def console_register():
    session_id = multiplexer.registration_session_create()

    if (session_id == -1):
        return 'Bad Operation - operation unsuccessful', 400
    else:
        return 'OK - registration successful', 200, {'Set-Cookie': f"session_id={session_id}; Path=/console"}

def console_unregister():
    session_id = extract_cookie(connexion.request.cookies, 'session_id')

    success = multiplexer.registration_session_destroy(session_id)

    if session_id != None and success:
        return 'OK - session unregistered successfully', 200, {'Set-Cookie': "session_id="} #invalidate cookie
    else:
        return 'Bad Request - operation unsuccessful or session did not exist', 400, {'Set-Cookie': "session_id="} #invalidate cookie

# Rendering

def console_render():
    session_id = extract_cookie(connexion.request.cookies, 'session_id')
    if session_id == None:
        return 'Unauthenticated - unregistered consoles cannot obtain an entity list', 401
    
    state = multiplexer.registration_session_state(session_id)
    entity_list = multiplexer.registration_session_render(session_id)

    if state == None or entity_list == None:
        return 'Unauthenticated - unregistered consoles cannot obtain an entity list', 401

    return json.dumps({"gamestate":state, "list":entity_list}), 200

# Session

def user_sessionlist():
    return json.dumps(multiplexer.session_list()), 200

def user_join():
    return "X", 400

def user_leave(id):
    return f"id: {id}", 400

def user_currentsession(id):
    return f"id: {id}", 400

def user_start(id):
    return f"id: {id}", 400

# Gameplay

def user_movement(id, direction):
    return f"id: {id} direction: {direction}", 400

def user_place(id, entity_type):
    return f"id: {id} entity_type: {entity_type}", 400

def user_status(id):
    return f"id: {id}", 400