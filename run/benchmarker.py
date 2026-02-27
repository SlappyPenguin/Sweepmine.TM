import subprocess
import time

num_rows = int(input("Enter number of rows: "))
num_cols = int(input("Enter number of columns: "))
num_bombs = int(input("Enter number of bombs: "))
is_torus = int(input("Is the board a torus? (1 = yes, 0 = no): "))
is_weighted = int(input("Is the board weighted? (1 = yes, 0 = no): "))
num_games = int(input("Enter number of games to play: "))

file = "../build/weighted_torus_graded" if (is_torus or is_weighted) else "../build/normal_graded"
count_wins = 0
total_time = 0.0

for _ in range(num_games):
    start_time = time.time()
    
    grader_program = subprocess.Popen([file], stdout=subprocess.PIPE, stdin=subprocess.PIPE)
    grader_program.stdin.write(f"{num_rows} {num_cols} {num_bombs} {is_torus} {is_weighted} 0\n".encode())
    grader_program.stdin.flush()
    answer = grader_program.stdout.readline().decode().strip()
    
    end_time = time.time();
    total_time += end_time - start_time

    print("Game", str(_+1)+":", answer)
    if "win" in answer:
        count_wins += 1

win_rate = (count_wins / num_games) * 100
time_per_100 = (total_time * 100) / num_games

print(f"{win_rate:.2f} % win rate")
print(f"{time_per_100:.2f} seconds per 100 games")