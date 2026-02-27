import subprocess
import time

file = "../build/normal_graded"
level_map = {"easy" : 0, "medium" : 1, "hard" : 2}
level_info = {
    0 : (9, 9, 10),
    1 : (16, 16, 40),
    2 : (16, 30, 99) 
}

level_str = input("Enter level (easy/medium/hard): ").lower()
level = level_map.get(level_str)
num_games = int(input("Enter number of games to play: "))

count_wins = 0
total_time = 0.0

for _ in range(num_games):
    start_time = time.time()

    num_rows = level_info[level][0]
    num_cols = level_info[level][1]
    num_bombs = level_info[level][2]
    
    grader_program = subprocess.Popen([file], stdout=subprocess.PIPE, stdin=subprocess.PIPE)
    grader_program.stdin.write(f"{num_rows} {num_cols} {num_bombs} {0} {0} 0\n".encode())
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