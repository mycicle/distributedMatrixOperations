import json
import requests
import numpy as np

url = 'http://127.0.0.1:5000/client'
data = {
    'operator': 'multiply',
    'left_operand': json.dumps(np.ones(5).tolist()),
    'right_operand': json.dumps(np.array([1, 2, 3, 4, 5]).tolist())
}
print(data)
x = requests.post(url, data=data)
print(x.text)