
ifeq ($(OS),Windows_NT)
	ZIP=powershell /c Compress-Archive -Path .\base64\,.\http\ submission.zip
	RM=powershell /c rm
else
	ZIP=zip -r submission.zip base64 http
	RM=rm
endif

all: zip

zip:
	$(ZIP)

clean:
	$(RM) submission.zip