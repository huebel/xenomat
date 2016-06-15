
docker:
	docker build -t huebel/xenomat .
	docker create --name=test -p 8901:8901 huebel/xenomat
