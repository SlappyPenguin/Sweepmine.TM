'''
Extreme Tic-Tac-Toe visualiser

To use this, first run the same grader on debug mode (K = 0). Use the following input for N = 25
- Subtask 1: 25 1 0
- Subtask 2: 25 2 0

The grader will print a summary of the game to tictactoe.in. You can then run python3 visualiser.py.

Change board_size to increase/decrease the size of the display (measured in pixels)
Change file_name to read from a different file
'''

board_size = 750
file_name = "tictactoe.in"

import tkinter as tk
import sys

class GridSquares(tk.Tk):
    def __init__(self, N, moves):
        super().__init__()
        square_size = board_size//(N+1)

        self.title("Extreme Tic-Tac-Toe")
        self.rows = N
        self.cols = N
        self.moves = moves
        self.square_size = square_size
        self.canvas = tk.Canvas(self, width=board_size + 10 , height=board_size + 10)
        self.canvas.pack()
        self.letter_tags = []
        self.draw_grid()

        # play pause button
        self.is_playing = False
        self.play_pause_button = tk.Button(self, text="Play", command=self.toggle_play_pause)
        self.play_pause_button.pack()
        self.current_move = 0

        # speed slider
        self.speed_slider = tk.Scale(self, from_=1, to=100, orient=tk.HORIZONTAL, label="Speed", command=self.update_speed, length = board_size//3)
        self.speed_slider.pack()
        self.update_speed(50)
        self.speed_slider.set(50)

        # movement slider + buttons
        self.move_slider = tk.Scale(self, from_=0, to=len(self.moves), orient=tk.HORIZONTAL, label="Move", command=self.update_move, length = board_size//2)
        self.move_slider.pack()
        button_frame = tk.Frame(self)
        button_frame.pack()
        self.minus_button = tk.Button(button_frame, text="Prev", command=lambda: self.change_move(-1))
        self.minus_button.pack(side=tk.LEFT)
        self.plus_button = tk.Button(button_frame, text="Next", command=lambda: self.change_move(1))
        self.plus_button.pack(side=tk.LEFT)

        self.tick()

    def draw_grid(self):
        for i in range(self.rows):
            for j in range(self.cols):
                x0 = (j+1) * self.square_size + 5
                y0 = (i+1) * self.square_size + 5
                x1 = x0 + self.square_size
                y1 = y0 + self.square_size
                self.canvas.create_rectangle(x0, y0, x1, y1, outline="black")

        for i in range(self.rows):
            self.canvas.create_text(self.square_size//2 + 5, 5 + (i+1)*self.square_size + self.square_size//2, text=str(i), font=("Helvetica", self.square_size//2))
        for i in range(self.cols):
            self.canvas.create_text(5 + (i+1)*self.square_size + self.square_size//2, self.square_size//2 + 5, text=str(i), font=("Helvetica", self.square_size//2))
    
    def draw_upto(self, upto):
        for tag in self.letter_tags:
            self.canvas.delete(tag)
        self.letter_tags.clear()

        for move in range(upto):
            row = (self.moves[move][0]+1) * self.square_size + self.square_size/2 + 5
            col = (self.moves[move][1]+1) * self.square_size + self.square_size/2 + 5
            tag = self.canvas.create_text(col, row, text=self.moves[move][2], font=("Helvetica", self.square_size//2))
            self.letter_tags.append(tag)

    def change_move(self, delta):
        if self.current_move + delta >= 0 and self.current_move + delta <= len(self.moves):
            self.current_move += delta
            self.move_slider.set(self.current_move)
            self.draw_upto(self.current_move)

    def tick(self):
        if self.is_playing and self.current_move < len(self.moves):
            self.change_move(1)
        else:
            self.pause()
        self.after(self.interval, self.tick)

    def play(self):
        self.is_playing = True
        self.play_pause_button.config(text="Pause")

    def pause(self):
        self.is_playing = False 
        self.play_pause_button.config(text="Play")

    def update_speed(self, value):
        self.interval = int(1/(int(value)+3) * 3000)

    def update_move(self, value):
        self.current_move = int(value)
        self.draw_upto(self.current_move)

    def toggle_play_pause(self):
        if self.is_playing:
            self.pause()
        else:
            self.play()

if __name__ == "__main__":

    try:
        f = open(file_name, 'r')
    except:
        print(f"Could not open file {file_name}")
        sys.exit()

    N, num_moves = map(int, f.readline().split())
    moves = []
    for _ in range(num_moves):
        r, c, p = f.readline().strip().split()
        moves.append((int(r), int(c), p))
    f.close()
    
    app = GridSquares(N, moves)
    app.mainloop()