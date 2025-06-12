FROM espressif/idf:v5.2.2

RUN addgroup --gid 2000 builder && \
    adduser --uid 2000 --ingroup builder --home /home/builder --shell /bin/bash --disabled-password --gecos "" builder
WORKDIR /home/builder

RUN chown builder:builder /opt/esp

RUN . /opt/esp/idf/export.sh

USER builder
