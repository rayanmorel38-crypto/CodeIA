// Orchestrateur central des tâches IA
#include "tasks/image_generation_task.h"
#include "tasks/video_generation_task.h"
#include "tasks/audio_generation_task.h"
#include "tasks/complex_task.h"

void run_all_tasks() {
    ImageGenerationTask imgTask;
    imgTask.run();

    VideoGenerationTask vidTask;
    vidTask.run();

    AudioGenerationTask audTask;
    audTask.run();

    ComplexTask cplxTask;
    cplxTask.run();
}

// Tu peux ajouter ici la logique d’orchestration, de gestion de file d’attente, etc.
