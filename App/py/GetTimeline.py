from atproto import Client
from atproto_client.models.app.bsky.feed.get_timeline import Response
import sys, json
args = sys.argv

# ログイン
api = Client()
api.login(args[1], args[2])

timeline = api.get_timeline()
jsondict = {}

count = 0
for data in timeline.feed:
    # リプライは除外
    if not data.post.record.reply is None:
        continue
    name = data.post.author.display_name
    text = data.post.record.text
    
    # jsonに5つだけ格納するように
    jsondict["timeline_" + str(count)] = {"name": name, "text": text}
    count += 1
    if count >= 5:
        break

# 適当に書き込み
path = './TimelineInfo.json'
f = open(path, 'w', encoding='utf-8')
json.dump(jsondict, f, ensure_ascii=False)
