#define BS 256

struct Complex{
	int re; //действительная чать
	int im; //мнимая
};

void readWord(char *buf){
    int i = 0;

    do{
        read(STDIN_FILENO, buf, sizeof(char));
    }while((buf[i] == '\0'  ||
            buf[i] ==  ' '  ||
            buf[i] == '\t'  ||
            buf[i] == '\n'    ) && i < BS - 1);

    do{
        read(STDIN_FILENO, buf + ++i, sizeof(char));
    }while (buf[i] != '\0' &&
            buf[i] !=  ' ' &&
            buf[i] != '\t' &&
            buf[i] != '\n' && i < BS - 1);
    buf[i] = '\0'; //терминирующее значение

    //printf("word = %s\n", buf);
}

int readInt(){
    char buf[BS];
    readWord(buf);
    return atoi(buf);
}

struct Complex readComplex(){
    int i = 0;
    struct Complex ret = {.re = 0, .im = 0};
    int signRe = 1, signIm = 1;
    char buf[BS]; for(int i = 0; i < BS; i++) buf[i] = '\0';
    readWord(buf);
    if(buf[0] == 'i') {ret.im = 1; ret.re = 0; return ret;}
    if(buf[0] == '-') {signRe = -1; i++;}
    for(; i < BS; i++){
        if(buf[i] < '0' || buf[i] > '9') break;
        ret.re = 10 * ret.re + (buf[i] - '0');
    }
    ret.re *= signRe;

    if(buf[i] == 'i'){
        ret.im = ret.re;
        ret.re = 0;
        //printf("ret = %d %d\n",ret.re,ret.im);
        return ret;
    }
    if(buf[i] == '-') {signIm = -1;}
    i++;
    if(buf[i] == 'i') {ret.im = 1;}
    
    for(; i < BS; i++){
        if(buf[i] < '0' || buf[i] > '9') break;
        ret.im = 10 * ret.im + (buf[i] - '0');
    }
    ret.im *= signIm;
    return ret;
}

void writeChar(char c){
    write(STDOUT_FILENO, &c, sizeof(char));
}

void writeInt(int num){
    if (num < 0) { writeChar('-'); num = -num; }
    char buf[BS], t;
    int len = 0;
    do{
        buf[len++] = '0' + num % 10;
        num /= 10;
    }while (num > 0);

    for (int i = 0; i < len / 2; i++){
        t = buf[i];
        buf[i] = buf[len - i - 1];
        buf[len - i - 1] = t;
    }
    write(STDOUT_FILENO, buf, len * sizeof(char));
}

void writeComplex(struct Complex x){
    if(x.re) writeInt(x.re);

    if(x.re && x.im > 0){
        writeChar('+');
    }
    
    if(x.im){
        writeInt(x.im);
        writeChar('i');
    }

    if(!x.re && !x.im){
        writeInt(0);
    }
}

void writeString(char *str){
    int len = 0;
    while (str[len++])
    ;
    write(STDOUT_FILENO, str, len * sizeof(char));
}

void error(char *str, int code){
    int len = 0;
    while (str[len++])
    ;
    write(STDERR_FILENO, "Error: ", 8 * sizeof(char));
    write(STDERR_FILENO, str, len * sizeof(char));
    writeChar('\n');
    exit(code);
}