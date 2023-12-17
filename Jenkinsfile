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
        sh 'ls -a'
        sh 'pwd'
        sh '. /.venv/bin/activate'
        sh 'pip show defectguard'
        sh 'defectguard -models deepjit -dataset platform -repo .   -uncommit -top 9 -main_language C -sort'
      }
    }

  }
}