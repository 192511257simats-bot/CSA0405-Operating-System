# CSA0405-<RegisterNumber>-Assignment5
Name: <your name>  Register No: <number>
OS: Ubuntu 24.04.5 LTS | bash 5.2 | Python 3.12.3 | gcc 13.3 | nginx 1.24 | gunicorn 22 | ab 2.3
Run: bash Task_B1/log_summary.sh ; bash Task_B1/disk_alert.sh 80
     cd Task_B2 && gunicorn -b 127.0.0.1:8000 app:app ; ab -n 5000 -c 50 http://127.0.0.1:8000/ping
     docker build -t result-portal Task_B2 ; kubectl apply -f Task_B3/deployment.yaml
Commit once per task: git add Task_B1 && git commit -m "B1 scripts and outputs"
