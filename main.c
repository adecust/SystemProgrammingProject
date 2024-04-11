#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define MAX_PATH_LEN 1024
#define MAX_COMMAND_LEN 124
#define MAX_CONTENT_LEN 1024

void dosya_sil(char *dosya_adi) {
    // Dosyayı silmek için remove fonksiyonunu kullanabiliriz
    if (remove(dosya_adi) == 0) {
        printf("%s dosyasi basariyla silindi.\n", dosya_adi);
    } else {
        printf("Dosya silinemedi veya bulunamadi.\n");
    }
}

void dosya_turu_degistir(char *dosya_adi, char *yeni_tur) {
    // Dosya adının uzunluğunu bul
    size_t uzunluk = strlen(dosya_adi);
    // Dosya adının uzunluğundan geriye doğru giderek "." karakterini bul
    int i;
    for (i = uzunluk - 1; i >= 0; i--) {
        if (dosya_adi[i] == '.') {
            // "." karakterinin konumunu bulduk, şimdi dosya adının uzantısını değiştirelim
            // Yeni dosya adını oluştur
            char yeni_dosya_adi[256]; // Örnek bir boyut, dosya adının maksimum uzunluğuna göre ayarlayın
            strncpy(yeni_dosya_adi, dosya_adi, i); // "." karakterinden önceki kısmı kopyala
            yeni_dosya_adi[i] = '\0'; // Yeni dosya adının sonunu belirle
            strcat(yeni_dosya_adi, "."); // "." karakterini ekle
            strcat(yeni_dosya_adi, yeni_tur); // Yeni uzantıyı ekle
            // Dosya adını değiştir
            if (rename(dosya_adi, yeni_dosya_adi) == 0) {
                printf("Dosya turu degistirildi : %s\n", yeni_dosya_adi);
            } else {
                printf("Dosya adı değiştirilemedi: %s\n", strerror(errno));
            }

            return;
        }
    }

    // "." karakteri bulunamadıysa hata mesajı ver
    printf("Uzantı bulunamadı.\n");
}


void dosya_duzenle(const char *dosya_adi) {
    char komut[1024];
    // Varsayılan metin editörü kullanarak dosyayı aç
    snprintf(komut, sizeof(komut), "xdg-open %s", dosya_adi); // Unix/Linux için

    system(komut);
}

void helpmenu() {
    printf("ls : dosya listeleme komutu\n");
    printf("cd.. : bir onceki dizine gitme komutu\n");
    printf("q : programdan cikma komutu\n");
    printf("duzenle : duzenleme yapmak icin metin editoru acma komutu\n");
    printf("open : dosya acma komutu\n"
           "---- Kullanım şekli  :   open-dosya_adi\n");
    printf("remove  : dosya silme komutu\n");
    printf("write : dosya yazma komutu\n");
    printf("copy : dosya kopyalama komutu\n");
    printf("changetype : dosya tipini degistirme komutu\n"
           "----Kullanım şekli : changetype-dosya.uzanti    devamında ise c,txt,pdf vs.");
    printf("mkdir : dizin olusturma komutu\n");
    printf("rmdir : dizin silme komutu\n");
    printf("chdir : dizin degiştirme komutu\n\n");
}

void dosya_kopyala(char *kaynak_dosya, char *hedef_dosya) {
    FILE *kaynak, *hedef;
    char karakter;

    // Kaynak dosyayı okuma modunda aç
    kaynak = fopen(kaynak_dosya, "r");
    if (kaynak == NULL) {
        printf("Hata: Kaynak dosya açılamadı.\n");
        exit(1);
    }

    // Hedef dosyayı yazma modunda aç
    hedef = fopen(hedef_dosya, "w");
    if (hedef == NULL) {
        printf("Hata: Hedef dosya açılamadı.\n");
        exit(1);
    }

    // Kaynak dosyanın sonuna kadar karakterleri hedef dosyaya kopyala
    while ((karakter = fgetc(kaynak)) != EOF) {
        fputc(karakter, hedef);
    }

    // Dosyaları kapat
    fclose(kaynak);
    fclose(hedef);

    printf("Dosya kopyalandı.\n");
}

int dizin_olustur(char *dizin_adi) {
    // mkdir fonksiyonuyla yeni bir dizin oluştur
    if (mkdir(dizin_adi) == 0) {
        printf("'%s' dizini başarıyla oluşturuldu.\n", dizin_adi);
        return 0; // Başarı durumu
    } else {
        // mkdir başarısız olursa hata durumunu işle
        if (errno == EEXIST) {
            fprintf(stderr, "'%s' dizini zaten var.\n", dizin_adi);
        } else {
            perror("mkdir");
        }
        return -1; // Hata durumu
    }
}
void dizin_degistir(char *yeni_dizin) {
    if (chdir(yeni_dizin) != 0) {
        perror("Dizin değiştirme hatası");
        exit(EXIT_FAILURE);
    } else{
        printf("Dizin degistirildi");
    }
}
char *dosyaYolu(const char *dosyaAdi,char *tamYol) {
    // Geçerli çalışma dizini yolunu al
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        exit(EXIT_FAILURE);
    }

    // Dosya adını çalışma dizini yoluna ekle
    strcpy(tamYol, cwd);
    strcat(tamYol, "\\");
    strcat(tamYol, dosyaAdi);

    // Dosyanın var olup olmadığını kontrol et
    if (access(tamYol, F_OK) != -1) {
        return tamYol;
    } else {
        return NULL;
    }
}
void dizin_sil(char *dizin_adi) {
    DIR *dizin;
    struct dirent *dosya;
    struct stat dosya_bilgi;
    char yol[PATH_MAX];

    // Dizin aç
    dizin = opendir(dizin_adi);
    if (!dizin)
        perror("Dizin açılamadı");

    // Dizin içeriğini sil
    while ((dosya = readdir(dizin)) != NULL) {
        if (strcmp(dosya->d_name, ".") == 0 || strcmp(dosya->d_name, "..") == 0)
            continue;

        snprintf(yol, sizeof(yol), "%s/%s", dizin_adi, dosya->d_name);

        // Dosyanın veya dizinin türünü al
        if (stat(yol, &dosya_bilgi) == -1)
            perror("Dosya bilgisi alınamadı");

        // Eğer öğe bir dosya ise sil
        if (S_ISREG(dosya_bilgi.st_mode)) {
            if (unlink(yol) == -1)
                perror("Dosya silinemedi");
        }
            // Eğer öğe bir dizin ise, tekrar bu fonksiyonu çağırarak dizini sil
        else if (S_ISDIR(dosya_bilgi.st_mode)) {
            dizin_sil(yol);
        }
    }
    // Dizini kapat
    closedir(dizin);
    // Dizini sil
    if (rmdir(dizin_adi) == -1)
        perror("Dizin silinemedi");
    else
        printf("Dizin silindi: %s\n", dizin_adi);
}

void dosya_ac(char *dosya_adi) {
    // Dosyayı aç
    FILE *dosya = fopen(dosya_adi, "r");

    if (dosya == NULL) {
        printf("Hata: Dosya bulunamadı veya açılamadı.\n");
        return;
    }
    printf("Dosya  icerigi \n");
    // Dosyanın içeriğini oku ve ekrana yaz
    char karakter;
    while ((karakter = fgetc(dosya)) != EOF) {
        putchar(karakter);
    }
    printf("\n");
    // Dosyayı kapat
    fclose(dosya);

}

void dosya_yaz(char *dosya_adi, char *icerik) {
    // Dosyayı yazma modunda aç
    FILE *dosya = fopen(dosya_adi, "w");

    // Dosyanın açılıp açılmadığını kontrol et
    if (dosya == NULL) {
        printf("Dosya oluşturulamadı veya açılamadı.\n");
        return;
    }

    // İçeriği dosyaya yaz
    fprintf(dosya, "%s", icerik);

    // Dosyayı kapat
    fclose(dosya);

    printf("Dosya '%s' basariyla olusturuldu ve icerigi yazildi.\n", dosya_adi);
}

// Dosya türünü döndüren yardımcı bir fonksiyon
char *get_file_type(mode_t mode) {
    if (S_ISREG(mode))
        return "Dosya";
    else if (S_ISDIR(mode))
        return "Dizin";
    else if (S_ISCHR(mode))
        return "Özel karakter aygıtı";
    else if (S_ISBLK(mode))
        return "Özel blok aygıtı";
    else if (S_ISFIFO(mode))
        return "İsimli boru (FIFO)";
    else
        return "Bilinmeyen";
}

// Dosya boyutunu insan dostu bir biçimde döndüren yardımcı bir fonksiyon
char *format_size(off_t size) {
    static char buf[32];
    const char *units[] = {"B", "KB", "MB", "GB", "TB"};

    int i = 0;
    double nbytes = size;
    while (nbytes >= 1024 && i < sizeof(units) / sizeof(units[0]) - 1) {
        nbytes /= 1024;
        i++;
    }

    sprintf(buf, "%.1f %s", nbytes, units[i]);
    return buf;
}

// Değiştirilme tarihini insan dostu bir biçimde döndüren yardımcı bir fonksiyon
char *format_time(time_t time) {
    static char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&time));
    return buf;
}

void dosya_izinleri_ayarla(char *dosya_adi, char *izinler) {
    int izin;
    if (izinler[0] == 'r' && izinler[1] == 'w' && izinler[2] == 'x') {
        izin = S_IRWXU;
    } else if (izinler[0] == 'r' && izinler[1] == 'w') {
        izin = S_IRUSR | S_IWUSR;
    } else if (izinler[0] == 'r' && izinler[1] == 'x') {
        izin = S_IRUSR | S_IXUSR;
    } else if (izinler[0] == 'w' && izinler[1] == 'x') {
        izin = S_IWUSR | S_IXUSR;
    } else if (izinler[0] == 'r') {
        izin = S_IRUSR;
    } else if (izinler[0] == 'w') {
        izin = S_IWUSR;
    } else if (izinler[0] == 'x') {
        izin = S_IXUSR;
    } else {
        printf("Geçersiz izinler!\n");
        return;
    }

    if (chmod(dosya_adi, izin) == -1) {
        perror("chmod");
        exit(EXIT_FAILURE);
    } else {
        printf("%s dosyasının izinleri başarıyla güncellendi.\n", dosya_adi);
    }
}

void list_files() {
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;
    // Bulunulan dizini aç
    dir = opendir(".");
    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    printf("Ad                Tur     Boyut       Degistirilme Tarihi\n");
    // Dizin içerisindeki dosyaları tara
    while ((entry = readdir(dir)) != NULL) {
        // Dosya hakkında bilgileri al
        if (stat(entry->d_name, &fileStat) < 0) {
            perror("stat");
            exit(EXIT_FAILURE);
        }
        // Dosya adını, türünü, boyutunu ve değiştirilme tarihini yazdır
        printf("%-20s %-8s %-12s %s\n", entry->d_name, get_file_type(fileStat.st_mode),
               format_size(fileStat.st_size), format_time(fileStat.st_mtime));
    }
    closedir(dir);
}

int main(int argc, char **argv) {
    char path[MAX_PATH_LEN];
    char prev_path[MAX_PATH_LEN];
    char content[MAX_CONTENT_LEN];

    char *command;
    int secim;
    char *ptr;

    while (1) {
        printf("Merhaba. \n Programa hosgeldiniz tercihinizi yapin\n");
        printf("1- Kullanim komutlari hakkinda bilgi almak icin tuslayiniz\n");
        printf("2- Dosya gezginini acmak icin tuslayin (alternative verison)\n");
        scanf("%d", &secim);
        if (secim == 1) {
            helpmenu();
        } else if (secim ==
                   2) //bu kısımda cd.. komutunu ayrı bir fonksiyon olarak değil main fonksiyonda yapma sebebim veri kaybını engellemek yaptığım blackbox testlerinde veri kaybı oluyordu
        {
            if (getcwd(path, sizeof(path)) == NULL) {
                perror("getcwd");
                return 1;
            }
            strcpy(prev_path, path);

            printf("Mevcut klasor: %s\n\n", path);
            printf("Kullanim kodlari hakkinda yardim almak istiyorsaniz help cikmak icin q yaziniz : ");
            while (1) {
                fflush(stdout);

                char input[124];
                int length = strlen(input);
                command = (char *) malloc((length + 1) * sizeof(char));
                strcpy(command, input);

                if (scanf("%s", command) != 1) {
                    printf("Geçersiz komut!\n");
                    continue;
                } else if (strcmp(command, "ls") == 0) {
                    list_files();
                } else if (strcmp(command, "help") == 0) {
                    helpmenu();
                } else if (strncmp(command, "copy", 4) == 0) {
                    ptr = strchr(command, '-');
                    if (ptr != NULL) {
                        char dosya_ad[MAX_PATH_LEN];
                        ptr = strchr(command, '-');
                        if (ptr != NULL) {
                            printf("%s isimli dosyayı hangi dosyaya kopyalamak istiyorsun: ", ptr + 1);
                            scanf("%s", dosya_ad);
                            printf("\n %s isimli dosya %s isimli dosyaya kopyalanıyor ", ptr + 1, command);
                            dosya_kopyala(ptr + 1, dosya_ad);

                        }
                    }
                } else if (strcmp(command, "q") == 0) {
                    free(command);
                    break;
                } else if (strncmp(command, "open", 4) == 0) {
                    ptr = strchr(command, '-');
                    dosya_ac(ptr + 1);
                } else if (strncmp(command, "cat", 3) == 0) {
                    ptr = strchr(command, '-');
                    char* yol = dosyaYolu(ptr+1,path);
                    strncpy(path, yol, sizeof(path));
                    dizin_degistir(path);
                } else if (strncmp(command, "changetype", 10) == 0) {
                    char yenitur[15];
                    ptr = strchr(command, '-');
                    printf("Dosyanın yeni türü ne olsun(. , / gibi  karakterler kullanmayınız):  ");
                    scanf("%s", yenitur);
                    command = ptr + 1;
                    dosya_turu_degistir(command, yenitur);

                } else if (strncmp(command, "duzenle", 7) == 0) {
                    ptr = strchr(command, '-');
                    dosya_duzenle(ptr + 1);
                } else if (strncmp(command, "mkdir", 5) == 0) {
                    ptr = strchr(command, '-');
                    dizin_olustur(ptr + 1);
                } else if (strncmp(command, "rmdir", 5) == 0) {
                    ptr = strchr(command, '-');
                    dizin_sil(ptr + 1);
                } else if (strncmp(command, "izin", 4) == 0) {

                    char yeniizin[15];
                    ptr = strchr(command, '-');
                    printf("Dosyanın izni türü ne olsun(örn : wrx wrx wrx):  ");
                    scanf("%s", yeniizin);
                    command = ptr + 1;
                    dosya_turu_degistir(command, yeniizin);

                    dosya_duzenle(ptr + 1);
                } else if (strncmp(command, "remove", 6) == 0) {
                    ptr = strchr(command, '-');
                    dosya_sil(ptr + 1);
                } else if (strcmp(command, "cd..") == 0) {
                    if (chdir("..") == 0) {
                        if (getcwd(path, sizeof(path)) == NULL) {
                            perror("getcwd");
                            return 1;
                        }
                        printf("\nMevcut klasor: %s\n", path);
                    } else {
                        perror("Klasöre gidilemedi");
                        break; // Hata olursa döngüyü sonlandır
                    }
                } else if (strncmp(command, "write", 5) == 0) {
                    ptr = strchr(command, '-');
                    if (ptr != NULL) {
                        printf("%s isimli dosyaya ne yazmak istersiniz: ", ptr + 1);
                        fflush(stdin); // Önceki komuttan kalan girişi temizle
                        fgets(content, sizeof(content), stdin);
                        if ((strlen(content) > 0) && (content[strlen(content) - 1] == '\n'))
                            content[strlen(content) - 1] = '\0';
                        dosya_yaz(ptr + 1, content);

                    } else {
                        printf("Hatalı komut formatı!\n");
                    }
                    continue;
                } else {
                    printf("Hatali tuslama yapildi");
                }
            }
        } else {
            printf("Hatali tuslama yaptiniz programdan cikariliyorsunuz");
            break;
        }

    }
}
