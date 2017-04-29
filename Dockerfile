FROM norionomura/sourcekit:311

RUN apt-get -y update && apt-get -y install build-essential clang

COPY . /code/skit
WORKDIR /code/skit
RUN make install
RUN make clean
