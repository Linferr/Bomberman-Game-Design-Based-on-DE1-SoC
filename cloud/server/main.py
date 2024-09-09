import sys
import json
import time
from flask import Flask, request, make_response

if __name__ == "server.main":
    from server.multiplexer import Multiplexer
else:
    from multiplexer import Multiplexer

HOST, PORT = "0.0.0.0", 9000

app = Flask(__name__,
            static_url_path='',
            static_folder='../../user/template',
            template_folder='../../user/template')

#STATIC FILES
@app.route('/')
def index():
    return app.send_static_file('index.html')

@app.route('/index')
def index2():
    return app.send_static_file('index.html')

@app.route('/about')
def about():
    return app.send_static_file('about.html')

@app.route('/games')
def games():
    return app.send_static_file('games.html')

@app.route('/user')
def user():
    return app.send_static_file('user.html')

multiplexer = Multiplexer("main_mux")

def extract_cookie(name):
    if name not in request.cookies:
        return None

    value = request.cookies[name]

    if len(value) == 0:
        return None
    #print(f"Cookie extracted - {name} : {value}")
    return value

# INTERCOMPONENT ENDPOINTS

# Registration

@app.route("/console/register", methods=['POST'])
def console_register_post():
    session_id = multiplexer.registration_session_create()

    if (session_id == -1):
        return "[Bad Operation - operation unsuccessful]", 400
    else:
        return "[OK - registration successful]", 200, {'Set-Cookie': f"session_id={session_id}; Path=/console"}

@app.route("/console/unregister", methods=['POST'])
def console_unregister_post():
    session_id = extract_cookie('session_id')
    if session_id == None:
        return "[Unauthenticated - unregistered console]", 401
    else:
        session_id = int(session_id)

    success = multiplexer.registration_session_destroy(session_id)

    if session_id != None and success:
        return "[OK - session unregistered successfully]", 200, {'Set-Cookie': "session_id="} #invalidate cookie
    else:
        return "[Bad Request - operation unsuccessful or session did not exist]", 400

# Rendering

@app.route("/console/render", methods=['GET'])
def console_render_get():
    #start = time.monotonic_ns()
    #print(f"##### REQUEST RECIEVED: {start}")
    #extract cookie
    session_id = extract_cookie('session_id')
    if session_id == None:
        return "[Unauthenticated - unregistered consoles cannot obtain an entity list]", 401
    else:
        session_id = int(session_id)

    #print(f"##### MUX CALL: {time.monotonic_ns()}")
    render_string = multiplexer.registration_session_render(session_id)
    #print(f"##### MUX FINISHED: {time.monotonic_ns()}")

    if render_string == None:
        return "[Unauthenticated - unregistered consoles cannot obtain an entity list]", 401

    #print(f"##### RETURNING: {time.monotonic_ns()}")
    #print(f"##### TOTAL TIME: {time.monotonic_ns() - start}")

    return f"{render_string}", 200

# Session

@app.route("/user", methods=['POST'])
def user_post():
    body = request.get_json()
    multiplexer.user_create(body['username'], body['saltedhash_password'])
    return "OK", 200

@app.route("/user/login", methods=['POST'])
def user_login_post():
    body = request.get_json()
    gen_id = multiplexer.user_login(body['username'], body['saltedhash_password'])

    if gen_id == None:
        return "Bad Request - invalid login", 400
    else:
        return f"OK - login successful", 200, {'Set-Cookie': f"id={gen_id}; Path=/user"}

@app.route("/user/logout", methods=['GET'])
def user_logout_get():
    gen_id = extract_cookie('id')
    if gen_id == None:
        return "Unauthenticated", 401
    
    multiplexer.user_logout(gen_id)

    return "OK", 200, {'Set-Cookie': "id="} #invalidate cookie

@app.route("/user/sessionlist", methods=['GET'])
def user_sessionlist_get():
    return json.dumps(multiplexer.session_list()), 200

@app.route("/user/join/<int:session_id>", methods=['GET'])
def user_join_get(session_id):
    gen_id = extract_cookie('id')
    if gen_id == None:
        return "Unauthenticated", 401
    
    result = multiplexer.session_join(gen_id, session_id)
    if (result == 404):
        return "Not Found - specified session not found", 404
    elif (result == 400):
        return "Bad Request - session could not be joined", 400
    else:
        return f"OK - session {session_id} joined successfully", 200

@app.route("/user/leave", methods=['GET'])
def user_leave_get():
    gen_id = extract_cookie('id')
    if gen_id == None:
        return "Unauthenticated", 401
    
    if multiplexer.session_leave(gen_id):
        return "OK - session left", 200
    else:
        return "Bad Request", 400


@app.route("/user/currentsession", methods=['GET'])
def user_currentsession_get():
    gen_id = extract_cookie('id')
    if gen_id == None:
        return "Unauthenticated", 401
    
    result = multiplexer.session_info(gen_id)

    if result:
        return json.dumps(result), 200
    else:
        return "Bad Request", 400

@app.route("/user/start", methods=['GET'])
def user_start_get():
    gen_id = extract_cookie('id')
    if gen_id == None:
        return "Unauthenticated", 401
    
    if multiplexer.session_start(gen_id):
        return "OK - game starting", 200
    else:
        return "Bad Request", 400

# Gameplay

@app.route("/user/movement", methods=['POST'])
def user_movement_post():
    gen_id = extract_cookie('id')
    if gen_id == None:
        return "Unauthenticated", 401
    
    body = request.get_json()
    if 'direction' in body:
        multiplexer.gameplay_move(gen_id, body['direction'])
    return 'OK - processed', 200

@app.route("/user/place", methods=['POST'])
def user_place_post():
    gen_id = extract_cookie('id')
    if gen_id == None:
        return "Unauthenticated", 401
    
    body = request.get_json()
    if 'entity_type' in body:
        multiplexer.gameplay_place(gen_id, int(body['entity_type']))
    return 'OK - processed', 200

@app.route("/user/status", methods=['GET'])
def user_status_get():
    gen_id = extract_cookie('id')
    if gen_id == None:
        return "Unauthenticated", 401
    
    result = multiplexer.gameplay_status(gen_id)
    if result:
        return json.dumps(result), 200
    else:
        return "Unauthenticated", 401
    
#testing waypoints
@app.route("/test", methods=['GET'])
def test():
    ret = ""

    multiplexer.general_lk.acquire()

    tests = [
        "[sw;]",
        "[sa;p11223344;h1231;b512523534545;e512523534545;]",
        "[sa;pff112233;h0123;b612713514715;e612713514715;]",
        "[sa;pff11ff22;h0102;b712;e612;]",
        "[sa;p1122ff33;h0031;b612712614555;e712;]",
        "[sa;p11ffffff;h2000;]",
        "[sa;p12233445;h1320;]",
        "[sa;pffffffff;h1111;]",
        "[sa;p55ff66ff;h1230;b521;]",
        "[h1230;sf1234;]"
    ]

    ret = tests[multiplexer.general % len(tests)]
    
    multiplexer.general += 1
    
    multiplexer.general_lk.release()

    return ret
    # cookies = [f"{x}:{y}" for x, y in request.cookies.items()]
    
    # if request.method == 'GET':
    #     return f"OK - GET request received\ncookies: {cookies}", 200
    # elif request.method == 'POST':
    #     return f"OK - POST request received\ndata: {request.get_json()}\ncookies: {cookies}", 200

@app.route("/test/debug", methods=['GET'])
def debug_translations():
    return json.dumps({'translations':multiplexer.translations, 'users':multiplexer.users}), 200

if (__name__ == "__main__"):
    
    #process command line arguments
    arguments = {}
    for arg in sys.argv:
        k, *v = arg.split('=', 1)
        arguments[k] = v

        #process options
        if '--port' in arguments.keys():
            PORT = int(arguments['--port'][0])

    app.run(port=PORT)