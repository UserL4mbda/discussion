#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>

void child_process() {
  printf("ENFANT PID : %d\n", getpid());
  //Command a lancer:
  // pacat -r --file-format=wav file.wav
  printf("Enfant: Enregistrement sonore en cours...ctrl-c pour stopper\n");

  execlp("pacat", "pacat", "-r","--file-format=wav", "file.wav", NULL);
  perror("Erreur lors de l'exécution de pacat");
  exit(EXIT_FAILURE);
}

void readfile(char* filename, char* content, int MAX_SIZE){
  FILE *file;
  //char filename[100];
  //char content[MAX_SIZE];
  char ch;
  int i = 0;

  //printf("Enter the filename: ");
  //scanf("%s", filename);

  file = fopen(filename, "r");

  if (file == NULL) {
    printf("File not found or unable to open the file.\n");
    exit(1);
  }

  // Read contents from file
  while ((ch = fgetc(file)) != EOF && i < MAX_SIZE - 1) {
    content[i++] = ch;
  }
  content[i] = '\0'; // Null-terminate the string

  fclose(file);

  //printf("Content of the file \"%s\":\n%s\n", filename, content);

  return;
}

void signal_handler(int signal) {
  printf("Signal %d trapped.\n", signal);
}

int record(){
  pid_t pid;
  // Créer un processus enfant
  pid = fork();

  if (pid == -1) {
    perror("Erreur lors de la création du processus enfant");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    // Code du processus enfant
    child_process();
  } else {
    // Code du processus parent
    printf("PARENT PID : %d\n", getpid());
    printf("ENFANT devrait avoir le PID : %d\n", pid);
    printf("Trap du signal...\n");

    if (signal(SIGINT, signal_handler) == SIG_ERR) {
      perror("Failed to set SIGINT handler");
      return EXIT_FAILURE;
    }

    // Attendre la réception de SIGINT (Ctrl-C)
    pause();

    // Envoyer SIGINT (Ctrl-C) à pacat
    kill(pid, SIGINT);
    return 0;
  }
}

int main() {
  FILE *fp;
//  char* whisper_name = "keen_fermi";
  char* whisper_name  = "whisper";
  char* sound_file    = "file.wav";
  char* text_file     = "file.txt";
  char* ollama_url    = "http://localhost:11434/api/generate";
  char* response_file = "response.txt";
  char* ollama_model  = "dolphin-mistral";
  char* prepromptfile = "preprompt.txt";

  //Verifions la presence du fichier wav
  if(access(sound_file, F_OK) == -1){
    //Le fichier n'existe pas, il faut le creer
    record();
  }

  char action[10000];

  //copie du fichier wav dans le docker
  sprintf(action, "docker cp %s %s:/", sound_file, whisper_name);
  printf("Command: %s\n", action);
  system(action);
  printf("\n");

  //execution de whisper pour recuperer au format texte
  sprintf(action, "docker exec %s whisper %s --language French", whisper_name, sound_file);
  printf("Command: %s\n", action);
  system(action);
  printf("\n");

  //recuperation du fichier text dans le repertoire local
  sprintf(action, "docker cp %s:/%s .", whisper_name, text_file);
  printf("Command: %s\n", action);
  system(action);
  printf("\n");

  //Affichage de ce que whisper a compris
  sprintf(action, "cat %s", text_file);
  printf("Command: %s\n", action);
  system(action);
  printf("\n");

  //Posons la question a ollama
  //prompt=`cat file.txt`;curl http://localhost:11434/api/generate -s -d "{\"model\":\"dolphin-mistral\",\"stream\":false, \"prompt\":\"$prompt\"}" | jq -r '.response' > response.txt
  //Utilisation d'un preprompt pour compenser l'enregistrement sonore de mauvaise qualite

  //char * preprompt = "You are an artificial intelligence used in extreme environments, where the quality of communication is very poor and user requests may be altered. Your role is to respond to the most likely question. You must respond in French. QUESTION:";

  int MAX_SIZE = 2048;
  char preprompt[MAX_SIZE];
  readfile(prepromptfile, preprompt, MAX_SIZE);
  //Suppression de \n
  size_t pos = strcspn(preprompt, "\n");
  preprompt[pos] = '\0';
  
  sprintf(action, "prompt=`cat %s`;curl %s -s -d \"{\\\"model\\\":\\\"%s\\\",\\\"stream\\\":false, \\\"prompt\\\":\\\"%s $prompt\\\"}\" | jq -r '.response' > %s", text_file, ollama_url, ollama_model, preprompt, response_file);
  printf("Command: %s\n", action);
  system(action);
  printf("\n");

  //Affichage de l'inference d'Ollama
  sprintf(action, "cat %s", response_file);
  printf("Command: %s\n", action);
  system(action);
  printf("\n");

  //Diction de la response par espeak
  sprintf(action, "espeak -v fr -f %s", response_file);
  printf("Command: %s\n", action);
  system(action);
  printf("\n");

  return 0;
}

