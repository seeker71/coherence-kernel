# concept: selective-retention
signals = [2, 7, 1, 8, 2, 8]
threshold = 5
score_floor = 100
retained_floor = 3
score = 0
retained = 0
signals.each_with_index do |value, index|
  score += value * (index + 1)
  retained += 1 if value >= threshold
end
verdict = score >= score_floor && retained >= retained_floor ? "coherent" : "fragmented"
puts "#{verdict}:#{score}:#{retained}"
