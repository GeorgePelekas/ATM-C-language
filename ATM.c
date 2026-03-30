#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#define admin "PELEKAS"
#define admin_ps 5425
int eikosariko = 400;
int dekariko = 800;
int taliro = 800;
struct log
{
    unsigned short al;
    unsigned short pin;
    char eponymo[28];
    char onoma[28];
    float poso_logariasmou;
}Logariasmoi[1600];
struct trns
{
    float poso;
    char perigrafi[3];
    unsigned short al;
    unsigned char day,month,year;
}transactions;
void FileToStructs()
{   int count=0;
    FILE *fp;
    fp=fopen("C:/free time/ergo2/accounts","rb");
    if (!fp) 
    {
         perror("accounts");
         printf("Δεν υπάρχουν δεδομένα στο δυαδικό αρχείο το πρόγραμμα θα τερματίσει");
         exit(0);
    }
    fread(Logariasmoi,sizeof(struct log),1600,fp);
    fclose(fp);
}
void save_accounts_file(void)
{
FILE *fp = fopen("C:/free time/ergo2/accounts", "wb");
if (!fp) { perror("accounts"); return; }
fwrite(Logariasmoi, sizeof(struct log), 1600, fp);
fclose(fp);
}
void get_today(unsigned char *d, unsigned char *m, unsigned char *y)
{
time_t t = time(NULL);
struct tm *tmv = localtime(&t);
*d = (unsigned char)tmv->tm_mday;
*m = (unsigned char)(tmv->tm_mon + 1);
*y = (unsigned char)(tmv->tm_year % 100);
}
void append_transaction(const char code3[3], float amount, unsigned short al)
{
struct trns t;
t.poso = amount;
t.perigrafi[0] = code3[0];
t.perigrafi[1] = code3[1];
t.perigrafi[2] = code3[2];
t.al = al;
get_today(&t.day, &t.month, &t.year);
FILE *fp = fopen("C:/free time/ergo2/transactions", "ab");
if (!fp) return;
fwrite(&t, sizeof(t), 1, fp);
fclose(fp);
}
int binarySearch(struct log Logariasmoi[],int n,int key)
{
    int low=0;
    int high=n-1;
    while(low<=high)
    {
        int mid=(low+high)/2;
        if (Logariasmoi[mid].al==key)
        {
            return mid;
        }
        if(Logariasmoi[mid].al<key)
        {
            low =mid+1;
        }
        else
        {
            high=mid-1;
        }
    }
    return -1;
}
int login()
{   int tries =3;
    unsigned short user_input_AL;
    unsigned short user_input_ps;
    printf("                ΠΛΗΚΤΡΟΛΟΓΗΣΕ ΤΟΝ ΑΡΙΘΜΟ ΛΟΓΑΡΙΑΣΜΟΥ ΣΟΥ : ");
    scanf("%hu",&user_input_AL);
    int position = binarySearch(Logariasmoi, 1600, user_input_AL);
    if(position!=-1)
    {
           
       while (tries > 0) {
        putchar('\n');
        printf("                        ΠΛΗΚΤΡΟΛΟΓΗΣΕ ΤΟ PIN ΣΟΥ ----\b\b\b\b");
        scanf("%hu", &user_input_ps);
        putchar('\n');
        if (user_input_ps ==Logariasmoi[position].pin) {
            return position;
        }
        tries--;
        if (tries > 0)
            printf("                        ΕΧΕΙΣ ΑΛΛΕΣ %d ΠΡΟΣΠΑΘΕΙΕΣ\n", tries);
    }
    return -1; 
        }
    else printf("                Ο ΛΟΓΑΡΙΑΣΜΟΣ %d ΔΕΝ ΥΠΑΡΧΕΙ !!!",user_input_AL);
    return -1;
}
int login_without_ps()
{   
    unsigned short user_input_AL;
    printf("                ΠΛΗΚΤΡΟΛΟΓΗΣΕ ΤΟΝ ΑΡΙΘΜΟ ΛΟΓΑΡΙΑΣΜΟΥ ΣΟΥ : ");
    scanf("%hu",&user_input_AL);
    int position = binarySearch(Logariasmoi, 1600, user_input_AL);
    if(position!=-1)
    {
            return position;  // ΕΠΙΤΥΧΙΑ
        }
        else printf("                Ο ΛΟΓΑΡΙΑΣΜΟΣ %d ΔΕΝ ΥΠΑΡΧΕΙ !!!",user_input_AL);
    return -1;
}
int login_without_ps_noroutput(int position)
{
    int idx = binarySearch(Logariasmoi, 1600,position);
if (idx != -1) return idx;
printf(" Ο ΛΟΓΑΡΙΑΣΜΟΣ %hu ΔΕΝ ΥΠΑΡΧΕΙ !!!\n", position);
return -1;
}
void find_4_last_transactions(unsigned short al)
{
FILE *fp = fopen("C:/free time/ergo2/transactions", "rb");
if (!fp) 
{
    perror("transactions");
     return; 
}
if (fseek(fp, 0, SEEK_END) != 0)
 {
     fclose(fp); 
     return;
 }
long sz = ftell(fp);
if (sz < 0) 
{ 
    fclose(fp); 
    return; 
}
long recsz = (long)sizeof(struct trns);
if (recsz <= 0 || sz < recsz) {
printf(" ΔΕΝ ΥΠΑΡΧΟΥΝ ΚΙΝΗΣΕΙΣ.\n");
fclose(fp);
return;
}
long nrecs = sz / recsz;
struct trns last[4];
int found = 0;
for (long i = nrecs - 1; i >= 0 && found < 4; --i)
    {
    if (fseek(fp, i * recsz, SEEK_SET) != 0) break;
    struct trns t;
    if (fread(&t, sizeof(t), 1, fp) != 1) break;
    if (t.al == al) {
    last[found++] = t; 
    }
    }
fclose(fp);
if (found == 0) {
printf(" ΔΕΝ ΥΠΑΡΧΟΥΝ ΚΙΝΗΣΕΙΣ.\n");
return;
}
for (int i = 0; i < found; ++i)
{
    struct trns *t = &last[i];
    printf("                        %u/%u/%u %c%c%c %+.1f\n",
    (unsigned)t->day, (unsigned)t->month, (unsigned)t->year,
    t->perigrafi[0], t->perigrafi[1], t->perigrafi[2],
    t->poso);
    }
}
float today_withdrawals(unsigned short al)
{
    unsigned char d, m, y;
    get_today(&d, &m, &y);
    FILE *fp = fopen("C:/free time/ergo2/transactions", "rb");
    if (!fp) return 0.0f;
    float sum = 0.0f;
    struct trns t;
    while (fread(&t, sizeof(t), 1, fp) == 1)
    {
    if (t.al != al) continue;
    if (t.day != d || t.month != m || t.year != y) continue;
    if (t.perigrafi[0] == 'W' && t.perigrafi[1] == 'D' && t.perigrafi[2] == 'R')
    {
    if (t.poso < 0) sum += -t.poso;
    else sum += t.poso;
    }
}


fclose(fp);
return sum;
}
int choices_one_to_six()
{   printf("\n");
    int choice_one_to_sixUserinput;
    puts("                        ΠΕΙΡΑΜΑΤΙΚΟ ΑΤΜ ΤΜΗΜΑΤΟΣ ΠΛΗΡΟΦΟΡΙΚΗΣ");
    puts("            -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*");

puts("                1. ΕΝΗΜΕΡΩΣΗ                                    4. ΚΑΤΑΘΕΣΗ\n");
puts("                2. ΜΕΤΑΦΟΡΑ                                     5. ΠΛΗΡΩΜΕΣ\n");
puts("                3. ΑΝΑΛΗΨΗ                                      6. ΑΛΛΑΓΗ PIN\n");
printf("                     ΠΑΡΑΚΑΛΩ ΕΠΙΛΕΞΤΕ ΤΙ ΘΕΛΕΤΕ (1-6) _\b");
scanf("%i",&choice_one_to_sixUserinput);
while(choice_one_to_sixUserinput!=1 && choice_one_to_sixUserinput!=2 && choice_one_to_sixUserinput!=3 && choice_one_to_sixUserinput!=4 && choice_one_to_sixUserinput!=5 && choice_one_to_sixUserinput!=6 && choice_one_to_sixUserinput!= 101 && choice_one_to_sixUserinput!=102 && choice_one_to_sixUserinput!=103)
{
    system("cls");
        puts("                        ΠΕΙΡΑΜΑΤΙΚΟ ΑΤΜ ΤΜΗΜΑΤΟΣ ΠΛΗΡΟΦΟΡΙΚΗΣ");
    puts("            -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*");

puts("                1. ΕΝΗΜΕΡΩΣΗ                                    4. ΚΑΤΑΘΕΣΗ\n");
puts("                2. ΜΕΤΑΦΟΡΑ                                     5. ΠΛΗΡΩΜΕΣ\n");
puts("                3. ΑΝΑΛΗΨΗ                                      6. ΑΛΛΑΓΗ PIN\n");
printf("                     ΠΑΡΑΚΑΛΩ ΕΠΙΛΕΞΤΕ ΤΙ ΘΕΛΕΤΕ (1-6) _\b");
scanf("%i",&choice_one_to_sixUserinput);
}
putchar('\n');
return choice_one_to_sixUserinput;
}
void choice_A()
{
    int user_input;
    puts("                                ΤΙ ΑΚΡΙΒΩΣ ΕΝΗΜΕΡΩΣΗ ΘΕΛΕΤΕ;");
    puts("                -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\n");
    puts("        1. ΥΠΟΛΟΙΠΟ     2. ΤΕΛΕΥΤΑΙΕΣ ΚΙΝΗΣΕΙΣ  3. ΛΕΠΤΟΜΕΡΕΙΕΣ ΛΟΓΑΡΙΑΣΜΟΥ");
    putchar('\n');
    putchar('\n');
    putchar('\n');
    printf("                ΠΑΡΑΚΑΛΩ ΕΠΙΛΕΞΤΕ ΤΙ ΑΚΡΙΒΩΣ ΕΝΗΜΕΡΩΣΗ ΘΕΛΕΤΕ (1-3) _\b");
    scanf("%d",&user_input);
    if(user_input==1)
    {
        int pos=login();
        if(pos!=-1)
            {
            printf("                        ΤΟ ΥΠΟΛΟΙΠΟ ΤΟΥ ΛΟΓΑΡΙΑΣΜΟΥ ΕΙΝΑΙ : %.1f\n",Logariasmoi[pos].poso_logariasmou);
            }
    }
    if (user_input==2)
    {
        int pos=login();
        if(pos!=-1)
        {
            find_4_last_transactions(Logariasmoi[pos].al);
        }
    }
    if(user_input==3){
        int pos=login();
        if(pos!=-1)
        {
            putchar('\n');
            printf("                        ΤΑ ΣΤΟΙΧΕΙΑ ΤΟΥ ΛΟΓΑΡΙΑΣΜΟΥ ΕΙΝΑΙ\n\n");
            puts("                        *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*");
            printf("                        ΕΠΩΝΥΜΟ ΔΙΚΑΙΟΥΧΟΥ    : %s\n",Logariasmoi[pos].eponymo);
            printf("                        ONOMA ΔΙΚΑΙΟΥΧΟΥ      : %s\n",Logariasmoi[pos].onoma);
            printf("                        ΥΟΠΟΛΟΙΠΟ ΛΟΓΑΡΙΑΣΜΟΥ : %.1f\n",Logariasmoi[pos].poso_logariasmou);
        }
    }
}
void choice_B()
{   int bresacc;
    int pos=login();
    if(pos!=-1)
    {
        printf("                ΠΛΗΚΤΡΟΛΟΓΗΣΕ ΤΟΝ ΑΡΙΘΜΟ ΤΟΥ ΑΠΟΔΕΚΤΗ ΛΟΓΑΡΙΑΣΜΟΥ : ");
        scanf("%d",&bresacc);
        
        int pos_transfer=login_without_ps_noroutput(bresacc);
        if(pos_transfer!=-1)
        {
            printf("                ΠΛΗΚΤΡΟΛΟΓΗΣΕ ΤΟ ΠΟΣΟ ΠΟΥ ΘΑ ΜΕΤΑΦΕΡΕΙΣ : ");
            float posa_lefta;
            scanf("%f",&posa_lefta);
            if(posa_lefta<=Logariasmoi[pos].poso_logariasmou&&posa_lefta>=0)
            {
                Logariasmoi[pos].poso_logariasmou-=posa_lefta;
                Logariasmoi[pos_transfer].poso_logariasmou+=posa_lefta;
                append_transaction("TRN", -posa_lefta, Logariasmoi[pos].al);
                append_transaction("TRN", +posa_lefta, Logariasmoi[pos_transfer].al);
                save_accounts_file();
            }
            else{printf("                ΤΟ ΥΠΟΛΟΙΠΟ ΤΟΥ ΛΟΓΑΡΙΑΣΜΟΥ ΔΕΝ ΕΠΑΡΚΕΙ !!!\n");}

        }
        
    }
    
}
void choice_C()
{
    int analipsi;
    int xwritikotita_eikosariko;
    int xwritikotita_dekariko;
    int xwritikotita_taliro;
    int b20=eikosariko,b10=dekariko,b5=taliro;
    int pos=login();
    if(pos!=-1){
        printf("                        ΤΟ ΥΠΟΛΟΙΠΟ ΤΟΥ ΛΟΓΑΡΙΑΣΜΟΥ ΕΙΝΑΙ : %.1f\n\n",Logariasmoi[pos].poso_logariasmou);
        printf("                ΠΛΗΚΤΡΟΛΟΓΗΣΕ ΤΟ ΠΟΣΟ ΠΟΥ ΕΠΙΘΥΜΕΙΣ : ");
        scanf("%d",&analipsi);
        int requested=analipsi;
        if (analipsi%5!=0 || requested<=0)
        {
            printf("                        ΤΟ ΠΟΣΟ ΑΝΑΛΗΨΗΣ ΠΡΕΠΕΙ ΝΑ ΕΙΝΑΙ ΠΟΛΛΑΠΛΑΣΙΟ ΤΩΝ 5 ΕΥΡΩ !!!\n");
            return;
        }

        if (requested>Logariasmoi[pos].poso_logariasmou)
        {
            printf("                        ΤΟ ΥΠΟΛΟΙΠΟ ΤΟΥ ΛΟΓΑΡΙΑΣΜΟΥ ΔΕΝ ΕΠΑΡΚΕΙ !!!\n");
            return;
        }
        float used_today = today_withdrawals(Logariasmoi[pos].al);
        if (used_today + (float)requested > 600.0) {
            printf("        ΥΠΕΡΚΑΛΥΠΤΕΙΣ ΤΟ ΗΜΕΡΗΣΙΟ ΟΡΙΟ ΠΟΥ ΔΙΚΑΙΟΥΣΤΕ ΓΙΑ ΧΡΕΩΣΗ ΤΟΥ ΛΟΓΑΡΙΑΣΜΟΥ !!!\n");
            return;
        }
        if (analipsi%5==0)
        {
            int fail=0;
            if (analipsi>=20)
            {
                if(analipsi/20>eikosariko)
                {
                    analipsi-=20*eikosariko;
                    eikosariko=0;
                }
                else
                {
                    xwritikotita_eikosariko=analipsi/20; eikosariko-=xwritikotita_eikosariko;
                    analipsi%=20;
                }
            }
            if(10<=analipsi)
            {
                if (analipsi/10>dekariko)
                {
                    analipsi-=10*dekariko;
                    dekariko=0;
                }
                else
                {
                    xwritikotita_dekariko=analipsi/10; dekariko-=xwritikotita_dekariko;
                    analipsi%=10;
                }
            }
            if(5<=analipsi)
            {
                if(analipsi/5>taliro)
                {
                    analipsi-=5*taliro;
                    taliro=0;
                }
                else
                {
                    xwritikotita_taliro=analipsi/5; taliro-=xwritikotita_taliro;
                    analipsi%=5;
                }
            }
            if (analipsi!=0){
                fail=1;
            }
            if (fail){
                eikosariko=b20; dekariko=b10; taliro=b5;
                printf("                        ΤΟ ΑΤΜ ΔΕΝ ΜΠΟΡΕΙ ΝΑ ΣΧΗΜΑΤΙΣΕΙ ΤΟ ΠΟΣΟ ΜΕ ΤΑ ΔΙΑΘΕΣΙΜΑ ΧΑΡΤΟΝΟΜΙΣΜΑΤΑ !!!\n");
                return;
            }
            else{
                Logariasmoi[pos].poso_logariasmou-=requested;
                append_transaction("WDR", -(float)requested, Logariasmoi[pos].al);
                printf("                        ΤΟ ΥΠΟΛΟΙΠΟ ΤΟΥ ΛΟΓΑΡΙΑΣΜΟΥ ΔΙΑΜΟΡΦΩΘΗΚΕ ΣΕ : %.1f\n",Logariasmoi[pos].poso_logariasmou);
                save_accounts_file();   
            }
        }
    }
}
void choice_D()
{
    int pos = login_without_ps();
    if (pos != -1)
    {
        int times_to_make_requested20_=0, times_to_make_requested10_=0, times_to_make_requested5_=0;
        int requested;
        printf("                        ΤΟ ΥΠΟΛΟΙΠΟ ΤΟΥ ΛΟΓΑΡΙΑΣΜΟΥ ΕΙΝΑΙ : %.1f\n\n",
               Logariasmoi[pos].poso_logariasmou);
        printf("        ΠΛΗΚΤΡΟΛΟΓΗΣΕ ΤΟ ΠΟΣΟ ΠΟΥ ΕΠΙΘΥΜΕΙΣ ΝΑ ΚΑΤΑΘΕΣΕΙΣ ΠΡΕΠΕΙ ΝΑ ΕΙΝΑΙ ΠΟΛΛΑΠΛΑΣΙΟ ΤΩΝ 5 ΕΥΡΩ: ");
        scanf("%d", &requested);
        int final_requested = requested;
        int max20_;
        int max10_;
        int max5_;
        if (requested <= 0 || requested % 5 != 0)
        {
            printf("                        ΤΟ ΠΟΣΟ ΚΑΤΑΘΕΣΗΣ ΠΡΕΠΕΙ ΝΑ ΕΙΝΑΙ ΠΟΛΛΑΠΛΑΣΙΟ ΤΩΝ 5 ΕΥΡΩ !!!\n");
            return;
        }
        while (requested != 0)
        {
            if (requested >= 20)
            {
                max20_ = requested / 20;
                printf("\n                        ΠΟΣΑ 20ευρα ΘΑ ΚΑΤΑΘΕΣΕΙΣ : ");
                scanf("%d", &times_to_make_requested20_);

                if (times_to_make_requested20_ > max20_)
                {
                    printf("                ΥΠΑΡΧΕΙ ΚΑΠΟΙΟ ΛΑΘΟΣ, ΤΟ ΠΟΣΟ ΠΟΥ ΠΛΗΚΤΡΟΛΟΓΗΣΕΣ ΔΕΝ ΣΧΗΜΑΤΙΖΕΤΑΙ !!!\n");
                    continue;
                }
                else
                {
                    requested -= 20 * times_to_make_requested20_;
                    if (requested == 0)
                    {
                        Logariasmoi[pos].poso_logariasmou += final_requested;
                        printf("                ΤΟ ΥΠΟΛΟΙΠΟ ΤΟΥ ΛΟΓΑΡΙΑΣΜΟΥ ΤΩΡΑ ΕΓΙΝΕ : %.1f\n",Logariasmoi[pos].poso_logariasmou);
                               append_transaction("DEP", +(float)final_requested, Logariasmoi[pos].al);
                               save_accounts_file();
                               eikosariko+=times_to_make_requested20_;
                        return;
                    }
                }
            }
            if (requested >= 10)
            {
                max10_ = requested / 10;

                printf("\n                        ΠΟΣΑ 10ευρα ΘΑ ΚΑΤΑΘΕΣΕΙΣ : ");
                scanf("%d", &times_to_make_requested10_);

                if (times_to_make_requested10_ > max10_)
                {
                    printf("                ΥΠΑΡΧΕΙ ΚΑΠΟΙΟ ΛΑΘΟΣ, ΤΟ ΠΟΣΟ ΠΟΥ ΠΛΗΚΤΡΟΛΟΓΗΣΕΣ ΔΕΝ ΣΧΗΜΑΤΙΖΕΤΑΙ !!!\n");
                    continue;
                }
                else
                {
                    requested -= 10 * times_to_make_requested10_;
                    if (requested == 0)
                    {
                        Logariasmoi[pos].poso_logariasmou += final_requested;
                        printf("                ΤΟ ΥΠΟΛΟΙΠΟ ΤΟΥ ΛΟΓΑΡΙΑΣΜΟΥ ΤΩΡΑ ΕΓΙΝΕ : %.1f",
                               Logariasmoi[pos].poso_logariasmou);
                               append_transaction("DEP", +(float)final_requested, Logariasmoi[pos].al);
                               eikosariko+=times_to_make_requested20_;
                               dekariko+=times_to_make_requested10_;
                               save_accounts_file();
                        return;
                    }
                }
            }

            if (requested >= 5)
            {
                max5_ = requested / 5;
                printf("\n                        ΠΟΣΑ 5ευρα ΘΑ ΚΑΤΑΘΕΣΕΙΣ : ");
                scanf("%d", &times_to_make_requested5_);

                if (times_to_make_requested5_ > max5_)
                {
                    printf("                ΥΠΑΡΧΕΙ ΚΑΠΟΙΟ ΛΑΘΟΣ, ΤΟ ΠΟΣΟ ΠΟΥ ΠΛΗΚΤΡΟΛΟΓΗΣΕΣ ΔΕΝ ΣΧΗΜΑΤΙΖΕΤΑΙ !!!\n");
                    continue;
                }
                else
                {
                    requested -= 5 * times_to_make_requested5_;
                    if (requested == 0)
                    {
                        Logariasmoi[pos].poso_logariasmou += final_requested;
                        append_transaction("DEP", +(float)final_requested, Logariasmoi[pos].al);
                        save_accounts_file();
                        printf("                ΤΟ ΥΠΟΛΟΙΠΟ ΤΟΥ ΛΟΓΑΡΙΑΣΜΟΥ ΤΩΡΑ ΕΓΙΝΕ : %.1f\n",Logariasmoi[pos].poso_logariasmou);
                        eikosariko+=times_to_make_requested20_;
                        dekariko+=times_to_make_requested10_;
                        taliro+=times_to_make_requested5_;
                        return;
                    }
                }
            }
        }
    
    }
    

}
void choice_E()
{
    int user_input;
    printf("\n                                ΤΙ ΑΚΡΙΒΩΣ ΠΛΗΡΩΜΗ ΘΕΛΕΤΕ;\n");
    printf("                -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\n");
    printf("        1. ΕΝΟΙΚΙΟ              2. ΔΑΠΑΝΕΣ ΛΟΓΑΡΙΑΣΝΩΝ (π.χ. ΔΕΗ, ΥΔΡΕΥΣΗ, ΤΗΛΕΦΩΝΙΑ)\n\n\n");
    printf("                ΠΑΡΑΚΑΛΩ ΕΠΙΛΕΞΤΕ ΤΙ ΑΚΡΙΒΩΣ ΘΕΛΕΤΕ ΝΑ ΠΛΗΡΩΣΕΤΕ (1-2) _\b");
    scanf("%d",&user_input);
    if(user_input==1)
    {
        int pos=login_without_ps();
        if(pos!=-1)
        {   
            float ipsos_pliromis;
            printf("                ΠΛΗΚΤΡΟΛΟΓΗΣΕ ΤΟ ΥΨΟΣ ΤΗΣ ΠΛΗΡΩΜΗΣ : ");
            scanf("%f",&ipsos_pliromis);
            if(ipsos_pliromis<=Logariasmoi[pos].poso_logariasmou&&ipsos_pliromis>=0){
            Logariasmoi[pos].poso_logariasmou-=ipsos_pliromis;
            append_transaction("RNT", -ipsos_pliromis, Logariasmoi[pos].al);
            save_accounts_file();}
            else{
                printf("                ΛΑΘΑΣΜΕΝΟ ΠΟΣΟ\n");
            }
        return;}
        }
    if(user_input==2)
    {
        int pos=login_without_ps();
        if(pos!=-1)
        {
            float ipsos_pliromis;
            printf("                ΠΛΗΚΤΡΟΛΟΓΗΣΕ ΤΟ ΥΨΟΣ ΤΗΣ ΠΛΗΡΩΜΗΣ : ");
            scanf("%f",&ipsos_pliromis);
            if(ipsos_pliromis<=Logariasmoi[pos].poso_logariasmou&&ipsos_pliromis>=0){
            append_transaction("EXP", -ipsos_pliromis, Logariasmoi[pos].al);
            Logariasmoi[pos].poso_logariasmou-=ipsos_pliromis;
            save_accounts_file();}
            else{
                printf("                ΛΑΘΑΣΜΕΝΟ ΠΟΣΟ\n");
            }
        return;
        }
    }

}
void choice_F()
{
    unsigned short newpassword;
    int pos=login();
    if(pos!=-1)
    {
    printf("                        ΠΛΗΚΤΡΟΛΟΓΗΣΕ ΤΟ ΝΕΟ Pin : ----\b\b\b\b");
    scanf("%hu",&newpassword);
    printf("\n                        ΕΠΙΒΕΒΑΙΩΣΕ ΤΟ ΝΕΟ Pin : ----\b\b\b\b");
    unsigned short verifynp;
    scanf("%hu",&verifynp);
    if(newpassword==verifynp)
    {
        Logariasmoi[pos].pin=newpassword;
      save_accounts_file();
    }
}
}
int secret_choices()
{
    int input_ps;
    int mphkes=0;
    int tries=3;
    char input_namead[21];
    do
    {
        printf("                        ΔΩΣΕ ΣΩΣΤΑ ΜΕ ΛΑΤΙΝΙΚΟΥΣ ΚΕΦΑΛΑΙΟΥΣ ΧΑΡΑΚΤΗΡΕΣ ΤΟ ΕΠΩΝΥΜΟ ΣΟΥ --------------------\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
        scanf("%20s",input_namead);
    }while(strcmp(input_namead,admin)!=0);
    do
    {
        printf("\n                        ΠΛΗΚΤΡΟΛΟΓΗΣΕ ΤΟ PIN ΣΟΥ ----\b\b\b\b");
        scanf("%d",&input_ps);
        if(input_ps==admin_ps)
        {
            mphkes=1;
            return mphkes;
        }
        else{tries--;printf("\n                        ΕΧΕΙΣ ΑΛΛΕΣ %d ΠΡΟΣΠΑΘΕΙΕΣ\n", tries);}
    }while(tries>0);
    return mphkes;
}
void choice_101(){
    int result=secret_choices();
    if(result)
    {
        printf("                        ΤΑ 20ευρα EINAI %i, ΤΑ 10ευρα EINAI %i KAΙ ΤΑ 5ευρα ΕΙΝΑΙ %i\n",eikosariko,dekariko,taliro);
    }
}
void choice_102()
{
    int result=secret_choices();
    if(result)
    {
        printf("                        Η ΕΝΕΡΓΕΙΑ ΑΥΤΗ ΑΦΟΡΑ ΤΗΝ ΤΡΟΦΟΔΟΣΙΑ ΤΟΥ ΑΤΜ ΜΕ ΧΑΡΤΟΝΟΜΙΣΜΑΤΑ\n");
        int total=20*eikosariko+10*dekariko+5*taliro;
        if (total>=8000)
        {
            printf("ΤΟ ΣΥΝΟΛΙΚΟ ΠΟΣΟ ΠΟΥ ΥΠΑΡΧΕΙ ΕΙΝΑΙ %d\n",total); return;
        }
        else
        {
            if(total+7000>30000)
            {
                 printf("ΤΟ ΣΥΝΟΛΙΚΟ ΠΟΣΟ ΠΟΥ ΥΠΑΡΧΕΙ ΕΙΝΑΙ %d\n",total); return;
            }
            else
            {
                eikosariko+=100;
                dekariko+=250;
                taliro+=500;
            }
        }   
        
    }
}
void choice_103(){
    int result=secret_choices();
    if(result)
    {
        printf("                        Η ΕΠΙΛΟΓΗ ΑΥΤΗ ΘΑ ΚΛΕΙΣΕΙ ΟΡΙΣΤΙΚΑ ΤΟ ΣΥΣΤΗΜΑ");
        exit(0);
    }
    else{return;}
}
void clearscreen()
{
    printf("press any key to continue. . . .");
    _getch();
    system("cls");
}
int main()
{ 
    while(1)
    {
    FileToStructs();
    int return_From_Choice_one_to_six=choices_one_to_six();
    printf("\n");
    if(return_From_Choice_one_to_six==1){choice_A();clearscreen();}
    else if(return_From_Choice_one_to_six==2){choice_B();clearscreen();}
    else if(return_From_Choice_one_to_six==3){choice_C();clearscreen();}
    else if(return_From_Choice_one_to_six==4){choice_D();clearscreen();}
    else if(return_From_Choice_one_to_six==5){choice_E();clearscreen();}
    else if(return_From_Choice_one_to_six==6){choice_F();clearscreen();}
    else if(return_From_Choice_one_to_six==101){choice_101();clearscreen();}
    else if(return_From_Choice_one_to_six==102){choice_102();clearscreen();}
    else if(return_From_Choice_one_to_six==103){choice_103();}
    }
    return 0;
}