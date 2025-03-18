from flask import Flask, render_template, request, g
import sqlite3

app = Flask(__name__)

DATABASE = 'places.db'

def get_db():
    db = getattr(g, '_database', None)
    if db is None:
        db = g._database = sqlite3.connect(DATABASE)
    return db

@app.teardown_appcontext
def close_connection(exception):
    db = getattr(g, '_database', None)
    if db is not None:
        db.close()

@app.route('/', methods=['GET', 'POST'])
def index():
    db = get_db()
    cursor = db.cursor()

    if request.method == 'POST':
        place_id = request.form['place_id']
        cursor.execute('SELECT * FROM places WHERE id = ?', (place_id,))
        place = cursor.fetchone()
        return render_template('index.html', places=get_places(), selected_place=place)

    return render_template('index.html', places=get_places())

def get_places():
    db = get_db()
    cursor = db.cursor()
    cursor.execute('SELECT * FROM places')
    return cursor.fetchall()

if __name__ == '__main__':
    app.run(debug=True)
