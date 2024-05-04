from atproto import Client
import sys
args = sys.argv

# ログイン
api = Client()
api.login(args[1], args[2])

path = './loginInfo.txt'
f = open(path, 'w')
f.write('')
f.close()
