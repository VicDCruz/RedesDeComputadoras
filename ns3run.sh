#!/bin/bash

# force kill & run
docker rm -f ns3

docker run -it -p 9:9 -p 49153:49153 --name ns3 -w /usr/ns-allinone-3.26/ns-3.26/ \
        -v `pwd`/proyects/.:/usr/ns-allinone-3.26/ns-3.26/scratch/. -d vicdcruz/redesdecomputadoras