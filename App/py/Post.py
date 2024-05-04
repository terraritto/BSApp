from atproto import Client
import sys
args = sys.argv

# ログイン
api = Client()
api.login(args[1], args[2])
api.post(args[3])