# discussion

Fonctionnement:

record sert de workflow
Enregistrement du son (speach) -> whisper (speach to text) -> ollama (inference de la reponse) -> espeak (text to speach)

Pour l'enregistrement on utilise pulse audio
```
pacat -r file.wav
```

Pour le speach to text on utilise whisper, voir le Dockerfile
Build de l'image:
```
docker build -t whisper .
```

Lancement de whisper
```
docker run -d --name whisper whisper sleep infinity
```

Pour ollama, voir directement sur leur github

espeak s'installe facilement (voir la gestion des paquet de votre distribution)

record est un programme tres simple et n'a pas besoin de bibliotheques particulieres.
Compilation:
```
gcc -o record record.c
```

Record se lance ensuite simplement
```
./record
```

