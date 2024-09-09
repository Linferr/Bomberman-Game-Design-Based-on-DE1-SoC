import sqlite3 as sl
con = sl.connect('users.db')

with con:
    # con.execute("""
    #     CREATE TABLE user (
    #         name TEXT,
    #         salted TEXT
    #     );
    # """)
    data = con.execute("SELECT * FROM user")
    for row in data:
        print(row)
