from firebase import firebase
Firebase = firebase.FirebaseApplication('https://your_storage.firebaseio.com', None)
result = Firebase.get('/users', None)
print result