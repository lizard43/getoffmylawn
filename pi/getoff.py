#!/usr/bin/python

import Tkinter as tk
from BaseHTTPServer import BaseHTTPRequestHandler
import SocketServer as socketserver
import thread as thread
import unicornhat as unicorn

def start_webserver():
    while True:
        httpd.handle_request()

class handleRoutes(BaseHTTPRequestHandler):

  # Handler for the GET requests
  def do_GET(self):
    if (self.path == '/'):
        return self.sendResponse('{"nothing": "to see here"}', 200, 'application/json')

    if (self.path.startswith('/api/v1/')):

      if (self.path.endswith('dnd')):
        self.background = "red"
        app.updateBackground(self.background)
        app.sparkle(self.background)
        app.updateLabel("Please\nDo Not Disturb")

        return self.sendResponse('{"state": "dnd"}', 200, 'application/json')

      if (self.path.endswith('knock')):
        self.background = "yellow"
        app.updateBackground(self.background)
        app.sparkle(self.background)
        app.updateLabel("Please\nKnock")

        return self.sendResponse('{"state": "knock"}', 200, 'application/json')

      if (self.path.endswith('available')):
        self.background = "green"
        app.sparkle(self.background)
        app.updateBackground(self.background)
        app.updateLabel("Please\nCome In")

        return self.sendResponse('{"state": "available"}', 200, 'application/json')

    else:
      return self.sendResponse('Not found.', 404, 'text/plain')

  def sendResponse(self, res, status, type):
    self.send_response(status)
    self.send_header('Content-type', type)
    self.end_headers()
    self.wfile.write(res)
    return

class Application(tk.Frame):

    def start_server(self):
        thread.start_new_thread(start_webserver, ())

    def createWidgets(self):
        self.lab = tk.Label(self, text=message)
        self.lab.config(font=("Courier bold", 32))
        self.lab.pack({"side": "top"})

    def updateBackground(self, color):
        root["bg"] = color
        self.lab["bg"] = color      

    def updateLabel(self, text):
        self.lab["text"] = text      

    def sparkle(self, color):
        unicorn.brightness(0.1)
        width,height=unicorn.get_shape()

        for b in range(0, width):
           if (color == 'green'): 
              unicorn.set_pixel(0, b, 0, 255, 0)

           if (color == 'yellow'): 
              unicorn.set_pixel(0, b, 255, 255, 0)

           if (color == 'red'): 
              unicorn.set_pixel(0, b, 255, 0, 0)

        unicorn.show()

    def __init__(self, master=None):
        tk.Frame.__init__(self, master)
        self.pack(expand='yes')
        self.createWidgets()
        self.updateBackground(background)

PORT = 8080

background = "green"
message = "Please\nCome In"

Handler = handleRoutes
httpd = socketserver.TCPServer(("", PORT), Handler)

root = tk.Tk()
root.title("Get Off My Lawn")

screen_width = root.winfo_screenwidth()
screen_height = root.winfo_screenheight()

reso = str(screen_width) + "x" + str(screen_height)

root.geometry(reso)
root["bg"] = background

app = Application(master=root)
app.sparkle(background)

app.start_server()
app.mainloop()
