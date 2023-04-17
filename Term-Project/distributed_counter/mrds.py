from __future__ import annotations

from typing import Optional, Final

from redis.client import Redis

from base import Worker
from constants import IN, COUNT, FNAME


class MyRedis:
  def __init__(self):
    self.rds: Final = Redis(host='localhost', port=6379, password='pass',
                       db=0, decode_responses=False)
    self.rds.flushall()
    self.rds.xgroup_create(IN, Worker.GROUP, id="0", mkstream=True)

  def add_file(self, fname: str):
    self.rds.xadd(IN, {FNAME: fname})

  def get_file(self, name):
    stream_reply = self.rds.xreadgroup(Worker.GROUP, name, {IN: ">"}, 1)
    if(len(stream_reply) != 0):
      return stream_reply
    stream_reply = self.rds.xautoclaim(IN, Worker.GROUP, name, 1000)
    if(len(stream_reply) != 0):
      return stream_reply
    return None

  def top(self, n: int) -> list[tuple[bytes, float]]:
    return self.rds.zrevrangebyscore(COUNT, '+inf', '-inf', 0, n,
                                     withscores=True)