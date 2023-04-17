import logging
import os
import time
from typing import Any

from base import Worker
from mrds import MyRedis
from constants import COUNT, IN, FNAME

class WcWorker(Worker):
  def run(self, **kwargs: Any) -> None:
    rds: MyRedis = kwargs['rds']
    while True:
      file = rds.get_file(self.name)
      if file is None:
        #if(len(rds.rds.xpending(IN, Worker.GROUP)) != 0):
          #time.sleep(1)
          #continue
        break
      filename = file[0][1][0][1][FNAME]
      logging.info(f"Processing {filename}")
      word_count = {}
      with open(filename, 'r') as f:
        for line in f:
          for word in line.split():
            word_count[word] = word_count.get(word, 0) + 1
      for word, count in word_count.items():
        rds.rds.zincrby(COUNT, count, word)
      rds.rds.xack(IN, Worker.GROUP, file[0][1][0][0])
      logging.info(f"Done processing {filename}")
    logging.info("Exiting")
