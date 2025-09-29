board_size = board_width = board_height = 650
file_name = "sweepmine.out"
O = 0
FLAG = 101
BLANK = 100
BOMB = 102

import tkinter as tk
import sys

class GridSquares(tk.Tk):
    def __init__(self, R, C, moves, charges, final_grid):
        super().__init__()
        board_width = board_size * (C+1) // (max(R, C)+1)
        board_height = board_size * (R+1) // (max(R, C)+1)
        square_size = board_size // (max(R, C)+1)

        self.title("Sweepmine.TM")
        self.rows = R
        self.cols = C
        self.moves = moves
        self.charges = charges
        self.final_grid = final_grid
        self.square_size = square_size
        self.canvas = tk.Canvas(self, width=board_width + 20 , height=board_height + 20)
        self.canvas.pack()
        self.letter_tags = []
        self.rectangle_tags = []
        self.draw_grid()

        self.is_playing = False
        self.play_pause_button = tk.Button(self, text="Play", command=self.toggle_play_pause)
        self.play_pause_button.pack()
        self.current_move = 0
        self.is_final = False

        self.speed_slider = tk.Scale(self, from_=1, to=100, orient=tk.HORIZONTAL, label="Speed", command=self.update_speed, length = board_size//3)
        self.speed_slider.pack()
        self.update_speed(50)
        self.speed_slider.set(50)

        self.move_slider = tk.Scale(self, from_=0, to=len(self.moves) - 1, orient=tk.HORIZONTAL, label="Move", command=self.update_move, length = board_size//2)
        self.move_slider.pack()
        button_frame = tk.Frame(self)
        button_frame.pack()
        self.show_final_button = tk.Button(button_frame, text="Show Final", command=self.toggle_final)
        self.show_final_button.pack(side=tk.LEFT)
        self.minus_button = tk.Button(button_frame, text="Prev", command=lambda: self.change_move(-1))
        self.minus_button.pack(side=tk.LEFT)
        self.plus_button = tk.Button(button_frame, text="Next", command=lambda: self.change_move(1))
        self.plus_button.pack(side=tk.LEFT)

        self.bind("<Left>", lambda event: self.change_move(-1))
        self.bind("<Right>", lambda event: self.change_move(1))
        self.bind("<space>", lambda event: self.toggle_play_pause())

        #add small instructions in bottom left corner
        self.instructions = tk.Label(self, text="Left/Right arrow keys to step move, Space to play/pause")
        self.instructions.pack(side=tk.LEFT)

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
            self.canvas.create_text(self.square_size//2 + 5, 5 + (i+1)*self.square_size + self.square_size//2, text=str(i+1), font=("Helvetica", self.square_size//2))
        for i in range(self.cols):
            self.canvas.create_text(5 + (i+1)*self.square_size + self.square_size//2, self.square_size//2 + 5, text=str(i+1), font=("Helvetica", self.square_size//2))

        #draw charge values small in the corner of each square
        for i in range(len(charges)):
            for j in range(len(charges[0])):
                x0 = (j+1) * self.square_size + 5
                y0 = (i+1) * self.square_size + 5
                text_string = charges[i][j]
                self.canvas.create_text(x0 + self.square_size//3, y0 + self.square_size//5, text=text_string, font=("Helvetica", self.square_size//6))

        self.draw_upto(0)
    
    def show_final(self):
        for tag in self.letter_tags:
            self.canvas.delete(tag)
        for rect in self.rectangle_tags:
            self.canvas.delete(rect)
        self.letter_tags.clear()
        self.rectangle_tags.clear()

        for i in range(self.rows):
            for j in range(self.cols):
                x0 = (j+1) * self.square_size + 5
                y0 = (i+1) * self.square_size + 5
                text_string = self.final_grid[i][j]
                colour_dict = ["blue", "green", "red", "midnight blue", "maroon", "cyan", "black", "violet red"]
                colour = "black"
                if text_string == BLANK:
                    text_string = " "
                elif text_string == FLAG:
                    text_string = "🚩"
                    if full_grid[i][j] != FLAG:
                        colour = "red"
                elif text_string == BOMB:
                    text_string = "💣"
                elif text_string == 0:
                    colour = "gray50"
                else:
                    colour = colour_dict[(int(text_string) - 1) % 8]
                tag = self.canvas.create_text(x0 + self.square_size//2, y0 + self.square_size//2, text=text_string, font=("Helvetica", self.square_size//max(2, len(str(text_string)))), fill=colour)
                self.letter_tags.append(tag)
        
    def toggle_final(self):
        if self.is_final:
            self.draw_upto(self.current_move)
            self.show_final_button.config(text="Show Final")
        else:
            self.show_final()
            self.show_final_button.config(text="Show Moves")
        self.is_final = not self.is_final

    def draw_upto(self, upto):
        for tag in self.letter_tags:
            self.canvas.delete(tag)
        for rect in self.rectangle_tags:
            self.canvas.delete(rect)
        self.letter_tags.clear()
        self.rectangle_tags.clear()

        if upto != 0:
            rect = self.canvas.create_rectangle(self.moves[upto][0][2] * self.square_size + 5, self.moves[upto][0][1] * self.square_size + 5, (self.moves[upto][0][2] + 1) * self.square_size + 5, (self.moves[upto][0][1] + 1) * self.square_size + 5, outline="black", fill="gray80")
            self.rectangle_tags.append(rect)

        for i in range(self.rows):
            for j in range(self.cols):
                x0 = (j+1) * self.square_size + 5
                y0 = (i+1) * self.square_size + 5
                text_string = self.moves[upto][1][i][j]
                colour_dict = ["blue", "green", "red", "midnight blue", "maroon", "cyan", "black", "violet red"]
                colour = "black"
                if text_string == BLANK:
                    text_string = " "
                elif text_string == FLAG:
                    text_string = "🚩"
                    if full_grid[i][j] != FLAG:
                        colour = "red"
                elif text_string == BOMB:
                    text_string = "💣"
                elif text_string == 0:
                    colour = "gray50"
                else:
                    colour = colour_dict[(int(text_string) - 1) % 8]
                tag = self.canvas.create_text(x0 + self.square_size//2, y0 + self.square_size//2, text=text_string, font=("Helvetica", self.square_size//max(2, len(str(text_string)))), fill=colour)
                self.letter_tags.append(tag)

    def change_move(self, delta):
        if self.current_move + delta >= 0 and self.current_move + delta <= len(self.moves) - 1:
            self.current_move += delta
            self.move_slider.set(self.current_move)
            self.draw_upto(self.current_move)

    def tick(self):
        if self.is_playing and self.current_move < len(self.moves):
            self.change_move(1)
        else:
            self.pause()
        if self.current_move == len(self.moves) - 1:
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

full_grid = []
simulated_grid = []
is_shown = []
is_flagged = []
visited = []
T = 0
def get_neighbours(x, y):
    neighbours = []
    if T == 0:
        for i in range(-1, 2):
            for j in range(-1, 2):
                if i == 0 and j == 0:
                    continue
                if x + i >= 0 and x + i < len(simulated_grid) and y + j >= 0 and y + j < len(simulated_grid[0]):
                    neighbours.append((x+i, y+j))
    else:
        for i in range(-1, 2):
            for j in range(-1, 2):
                if i == 0 and j == 0:
                    continue
                neighbours.append(((x+i) % len(simulated_grid), (y+j) % len(simulated_grid[0])))
    return neighbours
def r_dfs(x, y):
    if is_flagged[x][y]:
        return
    visited[x][y] = 1
    if O:
        return
    if full_grid[x][y] != 0:
        return
    for i, j in get_neighbours(x, y):
        if not visited[i][j]:
            r_dfs(i, j)
def reveal_tile(x, y):
    for i in range(len(simulated_grid)):
        for j in range(len(simulated_grid[0])):
            visited[i][j] = 0
    r_dfs(x, y)
    for i in range(len(simulated_grid)):
        for j in range(len(simulated_grid[0])):
            if visited[i][j]:
                is_shown[i][j] = 1
def update_simulated_grid():
    for i in range(len(simulated_grid)):
        for j in range(len(simulated_grid[0])):
            if is_shown[i][j]:
                simulated_grid[i][j] = full_grid[i][j]
            elif is_flagged[i][j]:
                simulated_grid[i][j] = FLAG
            else:
                simulated_grid[i][j] = BLANK

if __name__ == "__main__":
    try:
        f = open(file_name, 'r')
    except:
        print(f"Could not open file {file_name}")
        sys.exit()

    R, C, N, O, D = map(int, f.readline().strip().split())
    full_grid = []
    for _ in range(R):
        row = list(map(int, f.readline().strip().split()))
        full_grid.append(row)

    charges = []
    for _ in range(R):
        row = list(map(int, f.readline().strip().split()))
        charges.append(row)
    
    simulated_grid = [[BLANK for _ in range(C)] for _ in range(R)]
    visited = [[0 for _ in range(C)] for _ in range(R)]
    is_shown = [[0 for _ in range(C)] for _ in range(R)]
    is_flagged = [[0 for _ in range(C)] for _ in range(R)]

    moves = [((0, 0, 0), [row.copy() for row in simulated_grid])]

    line = ""
    while True:
        line = f.readline().strip()
        if line == "DIE" or line == "WIN" or (not line):
            break
        p, r, c = map(int, line.split())
        if p == 0 and full_grid[r-1][c-1] != FLAG:
            reveal_tile(r-1, c-1)
        elif p == 1:
            is_flagged[r-1][c-1] = 1
        else:
            #current square becomes -3
            moves.append(((p, r, c), [[cell if (i, j) != (r-1, c-1) else BOMB for j, cell in enumerate(row)] for i, row in enumerate(simulated_grid)]))
            break

        update_simulated_grid()

        moves.append(((p, r, c), [row.copy() for row in simulated_grid]))

        if D:
            for _ in range(R):
                f.readline()

    final_grid = [[BLANK for _ in range(C)] for _ in range(R)]
    for i in range(R):
        for j in range(C):
            if full_grid[i][j] == FLAG:
                final_grid[i][j] = BOMB
            else:
                final_grid[i][j] = full_grid[i][j]

    f.close()
    
    app = GridSquares(R, C, moves, charges, final_grid)
    app.mainloop()