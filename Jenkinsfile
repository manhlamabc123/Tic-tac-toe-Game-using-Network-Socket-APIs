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
        sh '. /.venv/bin/activate && sudo chmod -R a+rwx /.venv &&  git config user.name manhlamabc123 && defectguard -models deepjit -dataset platform -repo . -uncommit -top 9 -main_language C -sort'
      }
    }

  }
}