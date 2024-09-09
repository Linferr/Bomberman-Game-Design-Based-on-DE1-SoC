/* Author: Shibo Jin
 * Refrence: CPEN322
 * Refrence URL: https://github.com/ubc-cpen322-2022/project-ShiboJin
 * Revised by: Shibo Jin
 * Team: CPEN391 Group 26
 */

/* Refrence URL: https://github.com/ubc-cpen322-2022/project-ShiboJin */
function emptyDOM(elem) {
    while (elem.firstChild) {
        elem.removeChild(elem.firstChild);
    }
}

/* Refrence URL: https://github.com/ubc-cpen322-2022/project-ShiboJin */
function createDOM(htmlString) {
    let template = document.createElement('template');
    template.innerHTML = htmlString.trim();
    return template.content.firstChild;
}

const profile = {
    username: "",
    password: "",
    cookie: ""
};

const Service = {
    origin: window.location.origin,

    getAllSessions: function () {
        return new Promise((resolve, reject) => {
            const xhr = new XMLHttpRequest();
            xhr.open("GET", Service.origin + "/user/sessionlist");
            xhr.onload = function () {
                if (xhr.status === 200) {
                    resolve(JSON.parse(xhr.responseText));
                } else {
                    reject(new Error(xhr.responseText));
                }
            }
            xhr.onerror = function () {
                reject(new Error(xhr.responseText));
            }
            xhr.send();
        });
    },

    joinSessions: function (session_id) {
        return new Promise((resolve, reject) => {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", `${Service.origin}/user/join/${session_id}`);
            xhr.onload = function () {
                if (xhr.status == 200) {
                    resolve(xhr.responseText);
                } else {
                    reject(new Error(xhr.responseText));
                }
            }
            xhr.onerror = function () {
                reject(new Error(xhr.responseText));
            }
            xhr.send();
        });
    },

    leaveSessions: function () {
        return new Promise((resolve, reject) => {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", Service.origin + "/user/leave");
            xhr.onload = function () {
                if (xhr.status == 200) {
                    resolve(xhr.responseText);
                } else {
                    reject(new Error(xhr.responseText));
                }
            }
            xhr.onerror = function () {
                reject(new Error(xhr.responseText));
            }
            xhr.send();
        });
    },

    getInfoSessions: function () {
        return new Promise((resolve, reject) => {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", Service.origin + "/user/currentsession");
            xhr.onload = function () {
                if (xhr.status == 200) {
                    resolve(JSON.parse(xhr.responseText));
                } else {
                    reject(new Error(xhr.responseText));
                }
            }
            xhr.onerror = function () {
                reject(new Error(xhr.responseText));
            }
            xhr.send();
        });
    },

    startSessions: function () {
        return new Promise((resolve, reject) => {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", Service.origin + "/user/start");
            xhr.onload = function () {
                if (xhr.status == 200) {
                    resolve(xhr.responseText);
                } else {
                    reject(new Error(xhr.responseText));
                }
            }
            xhr.onerror = function () {
                reject(new Error(xhr.responseText));
            }
            xhr.send();
        });
    },

    playerMovement: function (direction) {
        return new Promise((resolve, reject) => {
            var xhr = new XMLHttpRequest();
            xhr.open("POST", Service.origin + "/user/movement");
            //https://stackoverflow.com/questions/45090086/setting-content-type-application-json-in-header
            xhr.setRequestHeader("Content-Type", "application/json");
            xhr.onload = function () {
                if (xhr.status == 200) {
                    resolve(xhr.responseText);
                } else {
                    reject(new Error(xhr.responseText));
                }
            }
            xhr.onerror = function () {
                reject(new Error(xhr.responseText));
            }
            xhr.send(JSON.stringify(direction));
        });
    },

    playerPlace: function (data) {
        return new Promise((resolve, reject) => {
            var xhr = new XMLHttpRequest();
            xhr.open("POST", Service.origin + "/user/place");
            //https://stackoverflow.com/questions/45090086/setting-content-type-application-json-in-header
            xhr.setRequestHeader("Content-Type", "application/json");
            xhr.onload = function () {
                if (xhr.status == 200) {
                    resolve(xhr.responseText);
                } else {
                    reject(new Error(xhr.responseText));
                }
            }
            xhr.onerror = function () {
                reject(new Error(xhr.responseText));
            }
            xhr.send(JSON.stringify(data));
        });
    },

    playerStatus: function () {
        return new Promise((resolve, reject) => {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", Service.origin + "/user/status");
            xhr.onload = function () {
                if (xhr.status == 200) {
                    resolve(JSON.parse(xhr.responseText));
                } else {
                    reject(new Error(xhr.responseText));
                }
            }
            xhr.onerror = function () {
                reject(new Error(xhr.responseText));
            }
            xhr.send();
        });
    },

    userRegister: function (data) {
        return new Promise((resolve, reject) => {
            var xhr = new XMLHttpRequest();
            xhr.open("POST", Service.origin + "/user");
            //https://stackoverflow.com/questions/45090086/setting-content-type-application-json-in-header
            xhr.setRequestHeader("Content-Type", "application/json");
            xhr.onload = function () {
                if (xhr.status == 200) {
                    resolve(xhr.responseText);
                } else {
                    reject(new Error(xhr.responseText));
                }
            }
            xhr.onerror = function () {
                reject(new Error(xhr.responseText));
            }
            xhr.send(JSON.stringify(data));
        });
    },

    userLogin: function (data) {
        return new Promise((resolve, reject) => {
            var xhr = new XMLHttpRequest();
            xhr.open("POST", Service.origin + "/user/login");
            //https://stackoverflow.com/questions/45090086/setting-content-type-application-json-in-header
            xhr.setRequestHeader("Content-Type", "application/json");
            xhr.onload = function () {
                if (xhr.status == 200) {
                    resolve(xhr.responseText);
                } else {
                    reject(new Error(xhr.responseText));
                }
            }
            xhr.onerror = function () {
                reject(new Error(xhr.responseText));
            }
            xhr.send(JSON.stringify(data));
        });
    },

    userLogout: function () {
        return new Promise((resolve, reject) => {
            const xhr = new XMLHttpRequest();
            xhr.open("GET", Service.origin + "/user/logout");
            xhr.onload = function () {
                if (xhr.status === 200) {
                    resolve(xhr.responseText);
                } else {
                    reject(new Error(xhr.responseText));
                }
            }
            xhr.onerror = function () {
                reject(new Error(xhr.responseText));
            }
            xhr.send();
        });
    },

};

class Lobby {
    constructor() {
        this.sessions = {}
    }


    getSession(sessionId) {
        const session = this.sessions[sessionId];
        if (session == null) {

        } else {
            return session;
        }
    }

    addSession(id, name, waitlist) {
        this.sessions[id] = new Session(id, name, waitlist);

        if (this.onNewSession !== undefined) {
            this.onNewSession(this.sessions[id]);
        }
    }

    deleteSession(id) {
        this.sessions = this.sessions.splice(id, 1)

        if (this.onDeleteSession !== undefined) {
            this.onDeleteSession();
        }
    }
}

class LobbyView {
    constructor(session) {
        this.session = session;
        this.elem = createDOM(
        `<div class="content">
            <!--List of Sessions-->
            <ul class="session-list">
            <li>
                <a href="#/join">
                    <p>Session 1
                </a>
            </li>
            <li>
                <a href="#/join">
                    <p>Session 2
                </a>
            </li>
            <li>
                <a href="#/join">
                    <p>Session 3
                </a>
            </li>
            <li>
                <a href="#/join">
                    <p>Session 4
                </a>
            </li>
            </ul>
        </div>`);

        this.session.onNewSession = (session) => {
            var newsession = createDOM(
                `<li>
          <a href='#/join/${session.id}'>
            <p>${session.name}
          </a>
        </li>`);
            this.listElem.appendChild(newsession);
            this.redrawList();
        }

        this.session.onDeleteSession = () => {
            this.redrawList();
        }

        this.listElem = this.elem.querySelector("ul.session-list");
        this.redrawList();
    }

    redrawList() {
        emptyDOM(this.listElem);

        for (var index in this.session.sessions) {
            var room = createDOM(
            `<li>
                <a href='#/join/${this.session.sessions[index].id}'>
                    <p>${this.session.sessions[index].name}
                </a>
            </li>`);
            this.listElem.appendChild(room);
        }
    }
}

class Session {
    constructor(id, name, waitlist = []) {
        this.id = id;
        this.name = name;
        this.waitlist = waitlist;
        this.length = 0;
    }

    addPlayer(player_name, player_id) {
        const playerInfo = {
            playerName: player_name,
            playerID: player_id
        };

        this.waitlist.push(playerInfo);

        if (this.updatePlayer !== undefined) {
            this.updatePlayer();
        }
    }

    removePlayer(player_name, player_id) {
        const playerInfo = {
            playerName: player_name,
            playerID: player_id
        };
        const index = this.waitlist.indexOf(playerInfo);
        if (index > 0 && index < this.length) {
            this.waitlist.splice(index, 1);
        } else {
            console.log("Invalid index");
        }

        if (this.updatePlayer !== undefined) {
            this.updatePlayer();
        }
    }
}

class SessionView {
    constructor(session) {
        this.session = session;
        this.elem = createDOM(
        `<div class="content">
            <!--View of Waiting Room-->
            <div class = player-count> 
                <p>Number of Players in this session: ${this.session.length} <br>
            </div>
            <button class="start-button"> Start </button>
            <button class="leave-button"> Leave </button>
        </div>`
        );

        this.session.updatePlayers = () => {
            this.refreshSession();
        }

        this.buttonElem_Start = this.elem.querySelector(".start-button");
        this.buttonElem_Leave = this.elem.querySelector(".leave-button");

        this.buttonElem_Start.addEventListener("click", () => {
            this.userStart();
        });

        this.buttonElem_Leave.addEventListener("click", () => {
            this.userLeave();
        });

        setInterval(() => {
            this.refreshSession();
        }, 400);
    }

    refreshSession() {
        var playerCountElem = this.elem.querySelector(".player-count");
        playerCountElem.textContent = `Number of Players in this session: ${this.session.length}`;
    }

    userStart() {
        Service.startSessions().then((result) => {
                console.log(result);
            },
            (error) => {
                alert("Either you are not host of this session or the number of players does not meet the requirments");
                error;
            })
    }

    userLeave() {
        Service.leaveSessions().then((result) => {
                console.log(result);
                var url = window.location.href;
                var index = url.lastIndexOf('/');
                url = url.substring(0, index);
                index = url.lastIndexOf('/');
                console.log(url.substring(0, index));
                window.location.href = url.substring(0, index).replace("#", "");
            },
            (error) => {
                error;
            })
    }
}

class ControlView {
    constructor() {
        this.entity_type = 1;
        this.player_num = 0;
        this.lastMoveRequestTime = 0;
        this.lastPlaceRequestTime = 0;
        this.elem = createDOM(
        `<div class="content">
            <div class="container1">
            <div class="center">
                <div class="buttons">
                <div class="container0">
                    <img src="images/char1.png" alt="char1_yellow"/>
                </div>
                <button class="button up"><img src="../images/youjiantou-copy.png" alt=""/></button>
                <button class="button left"><img src="../images/youjiantou-copy-copy-copy.png" alt=""/></button>
                <button class="button right"><img src="../images/youjiantou.png" alt=""/></button>
                <button class="button down"><img src="../images/youjiantou-copy-copy.png" alt=""/></button>
                <button class="place"><img src="../images/Place.png" style="width: 30px" alt=""/></button>
                <button class="switch1">bombh</button>
                <button class="switch2">bomb</button>
                <button class="switch3">bombv</button>
                </div>
            </div>
            </div>
        </div>`);

        this.playerImage = this.elem.querySelector(".container0 img");
        this.buttonElem_Left = this.elem.querySelector(".button.left");
        this.buttonElem_Right = this.elem.querySelector(".button.right");
        this.buttonElem_Bottom = this.elem.querySelector(".button.down");
        this.buttonElem_Top = this.elem.querySelector(".button.up");
        this.buttonElem_Place = this.elem.querySelector(".place");
        this.buttonElem_Bombh = this.elem.querySelector(".switch1");
        this.buttonElem_Bomb = this.elem.querySelector(".switch2");
        this.buttonElem_Bombv = this.elem.querySelector(".switch3");

        this.buttonElem_Left.addEventListener("click", () => {
            var currentTime = Date.now();
            if (currentTime - this.lastMoveRequestTime > 500) {
                Service.playerMovement({direction: "left"}).then(
                    (result) => {
                        this.lastMoveRequestTime = currentTime;
                        console.log(result);
                    },
                    (error) => {
                    })
            }
        });

        this.buttonElem_Right.addEventListener("click", () => {
            var currentTime = Date.now();
            if (currentTime - this.lastMoveRequestTime > 500) {
                Service.playerMovement({direction: "right"}).then(
                    (result) => {
                        this.lastMoveRequestTime = currentTime;
                        console.log(result);
                    },
                    (error) => {
                    })
            }
        });

        this.buttonElem_Bottom.addEventListener("click", () => {
            var currentTime = Date.now();
            if (currentTime - this.lastMoveRequestTime > 500) {
                Service.playerMovement({direction: "down"}).then(
                    (result) => {
                        this.lastMoveRequestTime = currentTime;
                        console.log(result);
                    },
                    (error) => {
                    })
            }
        });

        this.buttonElem_Top.addEventListener("click", () => {
            var currentTime = Date.now();
            if (currentTime - this.lastMoveRequestTime > 500) {
                Service.playerMovement({direction: "up"}).then(
                    (result) => {
                        this.lastMoveRequestTime = currentTime;
                        console.log(result);
                    },
                    (error) => {
                    })
            }
        });

        this.buttonElem_Place.addEventListener("click", () => {
            var currentTime = Date.now();
            if (currentTime - this.lastPlaceRequestTime > 2000) {
                Service.playerPlace({entity_type: this.entity_type}).then(
                    (result) => {
                        this.lastPlaceRequestTime = currentTime;
                        console.log(result);
                    },
                    (error) => {
                    })
            }
        });

        this.buttonElem_Bombh.addEventListener("click", () => {
            this.entity_type = 3;
        });

        this.buttonElem_Bomb.addEventListener("click", () => {
            this.entity_type = 1;
        });

        this.buttonElem_Bombv.addEventListener("click", () => {
            this.entity_type = 2;
        });

        setInterval(() => {
            this.refreshControl();
        }, 400);

        this.initKeyBindings();
    }

    initKeyBindings() {
        window.addEventListener("keydown", (event) => {
            switch (event.key) {
                case "w":
                    this.buttonElem_Top.click();
                    break;
                case "a":
                    this.buttonElem_Left.click();
                    break;
                case "s":
                    this.buttonElem_Bottom.click();
                    break;
                case "d":
                    this.buttonElem_Right.click();
                    break;
                case "2":
                    this.buttonElem_Bomb.click();
                    break;
                case "1":
                    this.buttonElem_Bombh.click();
                    break;
                case "3":
                    this.buttonElem_Bombv.click();
                    break;
                case "p":
                case " ":
                    this.buttonElem_Place.click();
                    break;
            }
        });
    }

    refreshControl() {
        if (this.player_num == 0) {
          this.playerImage.src = "images/char1.png";
          this.playerImage.alt = "char1_yellow";
        } else if (this.player_num == 1) {
          this.playerImage.src = "images/char2.png";
          this.playerImage.alt = "char2_pink";
        } else if (this.player_num == 2) {
            this.playerImage.src = "images/char3.png";
            this.playerImage.alt = "char3_green";
        } else if (this.player_num == 3) {
            this.playerImage.src = "images/char4.png";
            this.playerImage.alt = "char4_blue";
        }
      }
      
}


class ProfileView {
    constructor() {
        this.elem = createDOM(
        `<div class="content" style="display: flex; flex-direction: column; justify-content: center; align-items: center; height: 100%;">
            <h2>Login</h2>
            <div class="col-md-8 contact-right-w3" style="display: flex; flex-direction: column; align-items: center;">
                <input type="text" class="text" placeholder="Name" required="">
                <input type="text" class="text" placeholder="Password" required="">
                <div class="button-row" style=" display: flex; justify-content: flex-end; gap: 10px; width: 100%;">
                    <input type="submit" class="more_btn" value="Register">
                    <input type="submit" class="more_btn" value="Login">
                    <input type="submit" class="more_btn" value="Logout">
                </div>
            </div>
            <div class="clearfix"></div>
        </div>`
        );

        this.nameInput = this.elem.querySelector('input[placeholder="Name"]');
        this.passwordInput = this.elem.querySelector('input[placeholder="Password"]');
        this.registerButton = this.elem.querySelector('input[value="Register"]');
        this.loginButton = this.elem.querySelector('input[value="Login"]');
        this.logoutButton = this.elem.querySelector('input[value="Logout"]');

        this.registerButton.addEventListener('click', () => {
            this.userRegistration();
        });
        this.loginButton.addEventListener('click', () => {
            this.userLogin();
        });
        this.logoutButton.addEventListener('click', () => {
            this.userLogout();
        });
    }

    userRegistration() {
        const user_name = this.nameInput.value;
        const user_password = this.passwordInput.value;
        this.nameInput.value = '';
        this.passwordInput.value = '';
        if ((user_name === '') || (user_password === '')) {
            alert("Error! Expected both username and password have at least 1 character");
        } else {
            Service.userRegister({username: user_name, saltedhash_password: user_password}).then((result) => {
                    alert(result + ", Account successfully created");
                },
                (error) => {
                    alert("Error! Account unsuccessfully created");
                    error;
                })
        }
    }

    userLogin() {
        const user_name = this.nameInput.value;
        const user_password = this.passwordInput.value;
        this.nameInput.value = '';
        this.passwordInput.value = '';
        let profile = {
            username: "",
            password: "",
            cookie: ""
        };
        const storedProfile = JSON.parse(localStorage.getItem('profile'));
        if (storedProfile) {
            profile = storedProfile;
        }

        if ((user_name === '') || (user_password === '')) {
            alert("Error! Expected both username and password have at least 1 character");
        } else if ((profile.username !== '') || (profile.password !== '')) {
            alert("Error! You currently have already login an account.");
        } else {
            Service.userLogin({username: user_name, saltedhash_password: user_password}).then((result) => {
                    profile.username = user_name;
                    profile.password = user_password;
                    profile.cookie = document.cookie;
                    localStorage.setItem('profile', JSON.stringify(profile));
                    alert(result + ", Account successfully login");
                },
                (error) => {
                    alert("Error! Account unsuccessfully login");
                    error;
                })
        }
    }

    userLogout() {
        this.nameInput.value = '';
        this.passwordInput.value = '';
        let profile = {
            username: "",
            password: "",
            cookie: ""
        };
        const storedProfile = JSON.parse(localStorage.getItem('profile'));
        if (storedProfile) {
            profile = storedProfile;
        }

        if ((profile.username === "") || (profile.password === "")) {
            alert("Error! You currently don't have any account to logout.");
        } else {
            Service.userLogout().then((result) => {
                    profile.username = "";
                    profile.password = "";
                    profile.cookie = "";
                    localStorage.setItem('profile', JSON.stringify(profile));
                    alert(result + ", Account successfully logout");
                },
                (error) => {
                    alert("Error! Account unsuccessfully logout");
                    error;
                })
        }
    }
}

function main() {

    const lobby = new Lobby();
    const lobbyView = new LobbyView(lobby);
    const session = new Session();
    const sessionView = new SessionView(session);
    const controlView = new ControlView();
    const profileView = new ProfileView();
    let profile = {
        username: "",
        password: "",
        cookie: ""
    };
    const storedProfile = JSON.parse(localStorage.getItem('profile'));
    if (storedProfile) {
        profile = storedProfile;
    }

    console.log(profile.username);
    console.log(profile.password);
    var lobbyInterval;

    function renderRoute(profile) {
        let pageContent;
        let pageView;
        const url = window.location.href;
        const path = url.split("/").slice(3);
        let sessionInterval, controlInterval;

        if (path[0] === "user") {
            pageView = document.getElementById("page-view");
            emptyDOM(pageView);
            pageContent = profileView.elem;
            pageView.appendChild(pageContent);
            clearInterval(lobbyInterval);
            clearInterval(sessionInterval);
            clearInterval(controlInterval);
        } else if ((profile.username === "") || (profile.password === "")) {
            alert("Authorization failed. You are currently not logging in an account");
            clearInterval(lobbyInterval);
            clearInterval(sessionInterval);
            clearInterval(controlInterval);
        } else {
            if (path[0] === "games" && path.length === 1) {
                lobbyInterval = setInterval(() => {
                    Service.getAllSessions()
                        .then(
                            (result) => {
                                for (let i = 0; i < result.length; i++) {
                                    const room = lobby.getSession(result[i]);
                                    if (room === undefined) {
                                        lobby.addSession(result[i], "Session-" + (result[i]).toString(), []);
                                    } else {
                                        lobby.sessions[result[i]].id = result[i];
                                        lobby.sessions[result[i]].name = "Session-" + (result[i]).toString();
                                    }
                                }
                            },
                            (error) => console.log(error));
                }, 1000);
                document.cookie = profile.cookie;
                pageView = document.getElementById("page-view");
                emptyDOM(pageView);
                pageContent = lobbyView.elem;
                pageView.appendChild(pageContent);
            }

            else if (path[1] === "join") {
                clearInterval(lobbyInterval);
                if (path[3] === "control") {
                    clearInterval(lobbyInterval);
                    clearInterval(sessionInterval);

                    pageView = document.getElementById("page-view");
                    emptyDOM(pageView);
                    pageContent = controlView.elem;
                    console.log(controlView.elem);
                    pageView.appendChild(pageContent);
                    clearInterval(sessionInterval);

                    // Wrap controlInterval with a 1-second delay using setTimeout
                    setTimeout(() => {
                        controlInterval = setInterval(() => {
                            Service.playerStatus()
                                .then(
                                    (result) => {
                                        controlView.player_num = result["id"];
                                        console.log(result);
                                        if (result["ranking"] != 0) {
                                            var url = window.location.href;
                                            var newUrl = url.split("/games")[0] + "/games";
                                            window.location.href = newUrl;
                                            Service.leaveSessions().then(
                                                (result) => {},
                                                (error) => {console.log(error)}
                                            );
                                            clearInterval(controlInterval);
                                            clearInterval(sessionInterval);
                                            alert("Game over, your rank is " + result["ranking"].toString());
                                        }
                                    },
                                    (error) => console.log(error));
                        }, 500);
                    }, 2000); // 1 second delay (1000 milliseconds)

                } else {
                    clearInterval(lobbyInterval);
                    clearInterval(controlInterval);

                    Service.joinSessions(parseInt(path[2]))
                        .then((result) => {
                            console.log(result);
                            console.log(document.cookie);
                        })
                        .catch((error) => {
                            clearInterval(sessionInterval);
                            clearInterval(controlInterval);
                            window.history.back(); // Redirect to previous page
                            console.error(error);
                        });

                    pageView = document.getElementById("page-view");
                    emptyDOM(pageView);
                    pageContent = sessionView.elem;
                    pageView.appendChild(pageContent);

                    sessionInterval = setInterval(() => {
                        Service.getInfoSessions()
                            .then(
                                (result) => {
                                    console.log(result);
                                    if (result['state'] == 'active'){
                                        var url = window.location.href;
                                        window.location.href = url + "#/control";
                                        clearInterval(sessionInterval);
                                    }
                                    lobby.sessions[result['session_id']].length = result['current_count'];
                                    sessionView.session.length = result['current_count'];
                                },
                                (error) => console.log(error));
                    }, 500);
                }
            } else {
                clearInterval(lobbyInterval);
                clearInterval(sessionInterval);
                clearInterval(controlInterval);
            }
        }
    }

    window.addEventListener("hashchange", renderRoute); // Add event listener for hashchange
    renderRoute(profile);
}

window.addEventListener("load", main, false);