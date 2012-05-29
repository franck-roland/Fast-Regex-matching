#include "struct.h"
#include "regex.h"
#include "manipulate.h"
#define couleur(param) printf("\033[%sm",param)

void showans(char* message, char* answer){
	char* next;
	char* prev=message;
	char* match;
	int len = strlen(message)+1;
	char* copy = (char*) malloc(len*sizeof(char));
	char* token = (char*) malloc(len*sizeof(char));
	
	memset(copy,0,len);
	while(1){
		memset(token,0,len);
		next = strchr(answer,'\n');
		strncat(token,answer,(unsigned int)(next-answer));
		match = strstr(message,token);
		if(match==NULL)
			break;	
		if(next==NULL)
			break;
		couleur("0");
		strncat(copy,message,(unsigned int)(match-message));
		printf("%s",copy);
		memset(copy,0,len);
		couleur("31");
		strncat(copy,match,strlen(token));
		printf("%s",copy);
		memset(copy,0,len);
		message = match + strlen(token);
		answer = next+1;
	}
	if(strlen(message)>0){
		couleur("0");
		printf("%s\n",message);
	}
	else{
		printf("\n");
	}
	couleur("0");
	free(copy);
	free(token);
}
int main(){
	char* errormsg;
	char tomatch[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque tristique nulla non justo ultrices tincidunt. Pellentesque sollicitudin erat id velit vestibulum adipiscing. Vestibulum eget turpis nulla. Vestibulum congue varius risus, vitae ultricies ante euismod et. Morbi venenatis nisi eu arcu ullamcorper suscipit. Morbi sit amet mi vel diam mollis semper in eu neque. Donec vitae purus non libero ultrices malesuada.";
	char regex[] = "Lorem.eu.(Donec.)(purus ).";
    int indFields;
    int totalfields=0;
    int exactlymatch = 0	;
    Fields fields[MaxFields];
	
	indFields = match(regex,tomatch,fields,&totalfields,exactlymatch);
	if(indFields>=0)
    {
	    char* answer = (char*) malloc((strlen(tomatch)*2+1)*sizeof(char));
	    memset(answer,0,(strlen(tomatch)*2+1));
        computeAlignement(fields,exactlymatch,indFields,answer,tomatch,1);
        
        showans(tomatch, answer);
        free(answer);
        return 0;
    }
	errormsg = (char*) malloc((strlen(regex)+512)*sizeof(char));      
    memset(errormsg,0,(strlen(regex)+512));
   	doerrormessage(errormsg,indFields);
   	printf("Error :%s\n",errormsg);
    dealloc((void **)&errormsg);
    return 1;

}


