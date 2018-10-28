#define PRIME 211
#define EOS '\0'

int hashpjw(char* s)
{
	char *p;
	unsigned h = 0, g;
	for(p=s; *p!=EOS; p=p+1)
	{
		h = (h<<4) + (*p);
		if(g = h & 0xf0000000)
		{
			h = h ^ (g>>24);
			h = h ^ g;
		}
	}
	
	return h % PRIME;
}
