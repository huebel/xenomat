
docker:
	docker build --no-cache -t huebel/xenomat .
	docker rm -f xenomat
	docker create --name=xenomat -p 8901:8901 huebel/xenomat
