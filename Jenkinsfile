pipeline {
  agent any
  stages {
    stage('Checkout code') {
      steps {
        git(url: 'https://github.com/manhlamabc123/Tic-tac-toe-Game-using-Network-Socket-APIs', branch: 'main')
      }
    }

    stage('install DefectGuard') {
      steps {
        sh 'pip install -i https://test.pypi.org/simple/ defectguard'
      }
    }

  }
}