require 'redis'

redis = Redis.new

result = redis.publish "MyZWave", ARGV.join(" ")

puts result.inspect
