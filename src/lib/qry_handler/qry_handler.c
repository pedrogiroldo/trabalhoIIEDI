#include "qry_handler.h"
#include "../commons/queue/queue.h"
#include "../file_reader/file_reader.h"
#include <string.h>

typedef struct {

} QryResults_t;

// private functions defined as static and implemented
// on the end of the file
static void execute_a_command();

void execute_qry_commands(FileData *file_data, char *output_path) {
  Queue file_lines_queue = get_file_lines_queue(file_data);
  while (!queue_is_empty(file_lines_queue)) {
    char *line = (char *)queue_dequeue(file_lines_queue);
    char *command = strtok(line, " ");

    if (strcmp(command, "a") == 0) {
      execute_a_command();
    } else if (strcmp(command, "d") == 0) {
    } else if (strcmp(command, "p") == 0) {
    } else if (strcmp(command, "cln") == 0) {
    }
  }
}

static void execute_a_command() {}