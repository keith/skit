FROM swift:5.1.4

COPY . /code/skit
WORKDIR /code/skit
RUN make install
RUN make clean
