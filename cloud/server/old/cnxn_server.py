import connexion

HOST, PORT = "localhost", 9000

app = connexion.FlaskApp(__name__, specification_dir='../design_docs/')
app.add_api('openapi_spec.yml')

if (__name__ == "__main__"):
    app.run(port=PORT)