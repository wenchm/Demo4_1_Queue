# Demo4_1_Queue

# create a new repository on the command line
echo "# Demo4_1_Queue" >> README.md
git init
git add README.md
git commit -m "first commit"
git branch -M master
git remote add origin https://github.com/wenchm/Demo4_1_Queue.git

git remote -v
git push -u origin master

# push an existing repository from the command line
git remote add origin https://github.com/wenchm/Demo4_1_Queue.git
git branch -M master
git push -u origin master

# add all files and dir
git status
git add .
git commit -m "origin"
git push -u origin master
