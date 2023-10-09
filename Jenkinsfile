pipeline {
  agent any
  stages {
    stage('Checkout code') {
      parallel {
        stage('Checkout code') {
          steps {
            git(url: 'https://github.com/manhlamabc123/Tic-tac-toe-Game-using-Network-Socket-APIs', branch: 'main')
          }
        }

        stage('Install pip') {
          steps {
            sh 'apt-get install python3-pip'
          }
        }

      }
    }

    stage('install DefectGuard') {
      steps {
        sh 'pip install -i https://test.pypi.org/simple/ defectguard'
      }
    }

  }
}