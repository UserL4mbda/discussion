FROM ubuntu:latest
RUN apt update
RUN apt upgrade -y
RUN apt install ffmpeg -y
RUN apt install python3 -y
RUN apt install python3-pip -y
#RUN apt install git-all -y
RUN pip install -U openai-whisper
#RUN pip install git+https://github.com/openai/whisper.git
RUN apt install rustc -y
RUN pip install setuptools-rust
#COPY q1.wav /q1.wav

