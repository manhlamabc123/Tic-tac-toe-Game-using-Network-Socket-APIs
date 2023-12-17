pipeline {
  agent any
  stages {
    stage('Checkout code') {
      steps {
        git(url: 'https://github.com/manhlamabc123/Tic-tac-toe-Game-using-Network-Socket-APIs', branch: 'main')
      }
    }

    stage('Run DefectGuard') {
      steps {
        sh 'cd Tic-tac-toe-Game-using-Network-Socket-APIs'
        sh '. .venv/bin/activate'
        sh 'defectguard -models deepjit -dataset platform -repo .   -uncommit -top 9 -main_language C -sort'
      }
    }

  }
}