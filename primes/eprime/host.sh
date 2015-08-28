WORKER_IP="192.168.2.3"

#Run the eprime on the host like normal, but in the bg so the script continues.
host_result=$(./run.sh &)

#Run the script on the worker node using netcat, again in the background.
nc ${WORKER_IP} 1234
worker_result=$(./run.sh &)

total_primes=host_result+worker_result
echo total_primes
