
ifeq ($(OS),Windows_NT)
	RM=powershell /c rm
else	
	RM=rm
endif
# choco install zip
# ^ if missing 
ZIP=ZIP=zip -r submission.zip base64 http

all: zip

zip:
	$(ZIP)

clean:
	$(RM) submission.zip