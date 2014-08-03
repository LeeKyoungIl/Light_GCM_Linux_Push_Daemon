#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

struct string {
    char *ptr;
    size_t len;
};

struct data {
    char traceAscii;
};

static void dump(const char *text, FILE *stream, unsigned char *ptr, size_t size, char nohex) {
    size_t i;
    size_t c;
    
    unsigned int width=0x10;
    
    if(nohex) {
        width = 0x40;
    }
    
    fprintf(stream, "%s, %10.10ld bytes (0x%8.8lx)\n", text, (long)size, (long)size);
    
    for(i=0; i<size; i+= width) {
        fprintf(stream, "%4.4lx: ", (long)i);
        
        if(!nohex) {
            for(c = 0; c < width; c++) {
                if(i+c < size) {
                    fprintf(stream, "%02x ", ptr[i+c]);
                } else {
                    fputs("   ", stream);
                }
            }
        }
        
        for(c = 0; (c < width) && (i+c < size); c++) {
            if (nohex && (i+c+1 < size) && ptr[i+c]==0x0D && ptr[i+c+1]==0x0A) {
                i+=(c+2-width);
                break;
            }
            
            fprintf(stream, "%c", (ptr[i+c]>=0x20) && (ptr[i+c]<0x80)?ptr[i+c]:'.');
            
            if (nohex && (i+c+2 < size) && ptr[i+c+1]==0x0D && ptr[i+c+2]==0x0A) {
                i+=(c+3-width);
                break;
            }
        }
        
        fputc('\n', stream); /* newline */
    }
    
    fflush(stream);
}

static int myTrace(CURL *handle, curl_infotype type, char *data, size_t size, void *userp) {
    struct data *config = (struct data *)userp;
    const char *text;
    (void)handle;
    
    switch (type) {
        case CURLINFO_TEXT:
            fprintf(stderr, "== Info: %s", data);
        default:
            return 0;
            
        case CURLINFO_HEADER_OUT:
            text = "=> Send header";
            break;
            
        case CURLINFO_DATA_OUT:
            text = "=> Send data";
            break;
            
        case CURLINFO_SSL_DATA_OUT:
            text = "=> Send SSL data";
            break;
            
        case CURLINFO_HEADER_IN:
            text = "<= Recv header";
            break;
            
        case CURLINFO_DATA_IN:
            text = "<= Recv data";
            break;
            
        case CURLINFO_SSL_DATA_IN:
            text = "<= Recv SSL data";
            break;
    }
    
    dump(text, stderr, (unsigned char *)data, size, config->traceAscii);
    
    return 0;
}

void initString(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len+1);
    
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    
    s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t new_len = s->len + size*nmemb;
    s->ptr = realloc(s->ptr, new_len+1);
    
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    
    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;
    
    return size*nmemb;
}

struct string send_post_request (char *authorizationKey, char *url, char *params) {
    CURL *curl;
    CURLcode res;
    struct string s;
    struct data config;
    
    config.traceAscii = 1;
    
    curl_global_init(CURL_GLOBAL_ALL);
    
    curl = curl_easy_init();
    
    if(curl) {
        struct curl_slist *headers = NULL;
        
        char *authorizationKeyBody = "Authorization:key=%s";
        const size_t authorizationKeyBodySize = strlen(authorizationKeyBody);
        const size_t authorizationKeySize =strlen(authorizationKey);
        const size_t authorizationKeyHeaderSize = authorizationKeyBodySize + authorizationKeySize + 1;
        
        char *authorizationKeyHeader = (char *) malloc(authorizationKeyHeaderSize);
        sprintf(authorizationKeyHeader, authorizationKeyBody, authorizationKey);
        
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "charsets: utf-8");
        headers = curl_slist_append(headers, authorizationKeyHeader);
        
        initString(&s);
        
        /* debug */
        if (strstr(PUSH_DEBUG, "true")) {
            curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, myTrace);
            curl_easy_setopt(curl, CURLOPT_DEBUGDATA, &config);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        }
        
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        #ifdef SKIP_HOSTNAME_VERIFICATION
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        #endif
        
        #ifdef SKIP_PEER_VERIFICATION
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        #endif
        
        //curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        
        res = curl_easy_perform(curl);
        
        if(res != CURLE_OK) {
      		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        
        free(authorizationKeyHeader);
        authorizationKeyHeader = NULL;
    }
    
    curl_global_cleanup();
	
    return s;
}
