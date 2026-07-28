# concept: selective-retention
signals = [2, 7, 1, 8, 2, 8]
threshold = 5
score_floor = 100
retained_floor = 3
score = 0
retained = 0
for index, value in enumerate(signals):
    score += value * (index + 1)
    if value >= threshold:
        retained += 1
verdict = "coherent" if score >= score_floor and retained >= retained_floor else "fragmented"
print(f"{verdict}:{score}:{retained}")
