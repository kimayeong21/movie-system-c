#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 상수 정의
#define MAX_MOVIES 100
#define MAX_USERS 50
#define MAX_RATINGS 100

// 구조체 정의
typedef struct {
    int id;
    char title[50];
    char genre[20];
    int release_year;
    char director[50];
    float ratings[MAX_RATINGS];
    char rating_users[MAX_RATINGS][20];
    int rating_count;
} Movie;

typedef struct {
    char username[20];
    char password[20];
} User;

// 함수 선언
void show_main_menu();
void login();
void signup();
void admin_mode();
void show_movie_list(char* username);
void movie_detail(int movie_id, char* username);
void rate_movie(char* username);
void search_movie_by_keyword(char* username);
void admin_menu();
void add_movie();
void modify_movie();
void delete_movie();
void login_failed();
void load_movies_from_file();
void save_movies_to_file();
void load_users_from_file();
void save_users_to_file();
void save_review_to_file(int movie_id, char* username, float rating);
void print_hidden_password(const char* password);

// 글로벌 변수
User users[MAX_USERS];
User admins[10] = { {"a123", "a1234"} };
Movie movies[MAX_MOVIES];
int user_count = 0;
int admin_count = 1;
int movie_count = 0;

// 메인 함수
int main() {
    int choice;
    load_users_from_file();
    load_movies_from_file();

    while (1) {
        show_main_menu();
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("잘못된 입력입니다. 다시 입력해주세요.\n");
            continue;
        }

        switch (choice) {
        case 1:
            login();
            break;
        case 2:
            signup();
            break;
        case 3:
            admin_mode();
            break;
        case 4:
            printf("프로그램을 종료하고 데이터를 저장합니다...\n");
            save_users_to_file();
            save_movies_to_file();
            printf("저장이 완료되었습니다. 프로그램을 종료합니다.\n");
            return 0;
        default:
            printf("잘못된 입력입니다. 다시 선택해주세요.\n");
        }
    }
}

// 비밀번호 숨김 처리
void print_hidden_password(const char* password) {
    if (strlen(password) == 0) {
        printf("(비밀번호 없음)");
        return;
    }
    printf("%c", password[0]);
    for (size_t i = 1; i < strlen(password); i++) {
        printf("*");
    }
}

// 사용자 데이터 로드
void load_users_from_file() {
    FILE* file = fopen("user.csv", "r");
    if (file == NULL) {
        printf("user.csv 파일이 없습니다. 새로 생성됩니다.\n");
        return;
    }
    while (fscanf(file, "%19[^,],%19s\n", users[user_count].username, users[user_count].password) == 2) {
        user_count++;
    }
    fclose(file);
}

// 사용자 데이터 저장
void save_users_to_file() {
    FILE* file = fopen("user.csv", "w");
    if (file == NULL) {
        printf("user.csv 파일 저장 실패\n");
        return;
    }
    for (int i = 0; i < user_count; i++) {
        fprintf(file, "%s,%s\n", users[i].username, users[i].password);
    }
    fclose(file);
}

// 영화 데이터 로드
void load_movies_from_file() {
    FILE* file = fopen("movie.csv", "r");
    if (file == NULL) {
        printf("movie.csv 파일이 없습니다. 새로 생성됩니다.\n");
        return;
    }
    char buffer[200];
    fgets(buffer, sizeof(buffer), file);
    while (fgets(buffer, sizeof(buffer), file)) {
        sscanf(buffer, "%d,%49[^,],%19[^,],%d,%49[^\n]",
            &movies[movie_count].id,
            movies[movie_count].title,
            movies[movie_count].genre,
            &movies[movie_count].release_year,
            movies[movie_count].director);
        movies[movie_count].rating_count = 0;
        movie_count++;
    }
    fclose(file);
}

// 영화 데이터 저장
void save_movies_to_file() {
    FILE* file = fopen("movie.csv", "w");
    if (file == NULL) {
        printf("movie.csv 파일 저장 실패\n");
        return;
    }
    fprintf(file, "영화ID,영화 제목,장르,개봉 연도,감독\n");
    for (int i = 0; i < movie_count; i++) {
        fprintf(file, "%d,%s,%s,%d,%s\n",
            movies[i].id,
            movies[i].title,
            movies[i].genre,
            movies[i].release_year,
            movies[i].director);
    }
    fclose(file);
}

// 리뷰 데이터 저장
void save_review_to_file(int movie_id, char* username, float rating) {
    FILE* file = fopen("review.csv", "a");
    if (file == NULL) {
        printf("review.csv 파일 저장 실패\n");
        return;
    }
    fprintf(file, "%d,%s,%.1f\n", movie_id, username, rating);
    fclose(file);
}

// 영화 평가
void rate_movie(char* username) {
    int movie_id;
    float rating;

    printf("평가할 영화의 ID를 입력하세요: ");
    if (scanf("%d", &movie_id) != 1) {
        while (getchar() != '\n');
        printf("잘못된 입력입니다.\n");
        return;
    }

    for (int i = 0; i < movie_count; i++) {
        if (movies[i].id == movie_id) {
            printf("평점을 입력하세요 (1.0 ~ 5.0): ");
            if (scanf("%f", &rating) != 1) {
                while (getchar() != '\n');
                printf("잘못된 입력입니다.\n");
                return;
            }

            if (rating < 1.0 || rating > 5.0) {
                printf("유효하지 않은 평점입니다. 1.0 ~ 5.0 사이의 값을 입력해주세요.\n");
                return;
            }

            movies[i].ratings[movies[i].rating_count] = rating;
            strcpy(movies[i].rating_users[movies[i].rating_count], username);
            movies[i].rating_count++;

            save_movies_to_file(); // 영화 데이터 저장
            save_review_to_file(movie_id, username, rating); // 리뷰 데이터 저장
            printf("평점이 성공적으로 저장되었습니다!\n");
            return;
        }
    }
    printf("해당 ID의 영화를 찾을 수 없습니다.\n");
}



// 메인 메뉴
void show_main_menu() {
    printf("==========================================\n");
    printf("          영화 추천 프로그램\n");
    printf("==========================================\n");
    printf("[1] 로그인\n[2] 회원가입\n[3] 관리자 모드\n[4] 종료\n");
    printf("------------------------------------------\n");
    printf("번호를 입력하여 선택하세요: ");
}

// 로그인
void login() {
    char username[20], password[20];
    printf("==========================================\n");
    printf("        로그인\n");
    printf("==========================================\n");
    printf("아이디를 입력하세요: ");
    scanf("%s", username);
    printf("비밀번호를 입력하세요: ");
    scanf("%s", password);

    for (int i = 0; i < user_count; i++) {
        if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0) {
            printf("로그인 성공! 사용자: %s, 비밀번호: ", username);
            print_hidden_password(password);
            printf("\n");
            show_movie_list(username);
            return;
        }
    }
    login_failed();
}

// 로그인 실패 처리
void login_failed() {
    int choice;
    printf("로그인 실패\n");
    printf("==========================================\n");
    printf("[1] 다시 로그인하기\n[2] 메인 화면으로 돌아가기\n[3] 종료\n");
    printf("번호를 입력하세요 >> ");
    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n');
        printf("잘못된 입력입니다.\n");
        login_failed();
    }
    if (choice == 1) login();
    else if (choice == 2) return;
    else exit(0);
}

// 회원가입
void signup() {
    char username[20], password[20];
    printf("==========================================\n");
    printf("        회원가입\n");
    printf("==========================================\n");
    printf("아이디를 입력하세요: ");
    scanf("%s", username);
    printf("비밀번호를 입력하세요: ");
    scanf("%s", password);

    strcpy(users[user_count].username, username);
    strcpy(users[user_count].password, password);
    user_count++;
    printf("회원가입 성공! 사용자: %s, 비밀번호: ", username);
    print_hidden_password(password);
    printf("\n");
}

// 영화 목록
void show_movie_list(char* username) {
    while (1) {
        printf("==========================================\n");
        printf("      전체 영화 목록\n");
        printf("==========================================\n");
        for (int i = 0; i < movie_count; i++) {
            printf("[%d] 영화 제목: %s (%d) - 장르: %s\n",
                movies[i].id, movies[i].title, movies[i].release_year, movies[i].genre);
        }
        printf("==========================================\n");
        printf("[1] 영화 상세 보기\n[2] 영화 평가하기\n[3] 영화 검색\n[4] 로그아웃\n");
        printf("번호를 입력하세요: ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("잘못된 입력입니다.\n");
            continue;
        }

        if (choice == 1) {
            int movie_id;
            printf("상세 정보를 볼 영화의 ID를 입력하세요: ");
            if (scanf("%d", &movie_id) != 1) {
                while (getchar() != '\n');
                printf("잘못된 입력입니다.\n");
                continue;
            }
            movie_detail(movie_id, username);
        }
        else if (choice == 2) {
            rate_movie(username);
        }
        else if (choice == 3) {
            search_movie_by_keyword(username);
        }
        else if (choice == 4) {
            break;
        }
        else {
            printf("잘못된 입력입니다. 다시 시도해주세요.\n");
        }
    }
}

// 영화 상세 정보
void movie_detail(int movie_id, char* username) {
    for (int i = 0; i < movie_count; i++) {
        if (movies[i].id == movie_id) {
            printf("==========================================\n");
            printf("영화 제목: %s\n", movies[i].title);
            printf("장르: %s\n", movies[i].genre);
            printf("개봉 연도: %d\n", movies[i].release_year);
            printf("감독: %s\n", movies[i].director);
            if (movies[i].rating_count > 0) {
                printf("평가 내역:\n");
                for (int j = 0; j < movies[i].rating_count; j++) {
                    printf(" - 사용자: %s, 평점: %.1f\n", movies[i].rating_users[j], movies[i].ratings[j]);
                }
            }
            else {
                printf("평가 내역이 없습니다.\n");
            }
            printf("==========================================\n");
            return;
        }
    }
    printf("해당 ID의 영화를 찾을 수 없습니다.\n");
}



// 영화 검색
void search_movie_by_keyword(char* username) {
    char keyword[50];
    int found = 0;

    printf("검색할 키워드를 입력하세요: ");
    scanf("%s", keyword);

    printf("==========================================\n");
    printf("      영화 검색 결과\n");
    printf("==========================================\n");
    for (int i = 0; i < movie_count; i++) {
        if (strstr(movies[i].title, keyword) || strstr(movies[i].genre, keyword)) {
            printf("[%d] 영화 제목: %s (%d) - 장르: %s\n",
                movies[i].id, movies[i].title, movies[i].release_year, movies[i].genre);
            found = 1;
        }
    }
    if (!found) printf("검색 결과가 없습니다.\n");
    printf("==========================================\n");
}

// 관리자 모드
void admin_mode() {
    char username[20], password[20];
    printf("==========================================\n");
    printf("          관리자 모드\n");
    printf("==========================================\n");
    printf("아이디를 입력하세요: ");
    scanf("%s", username);
    printf("비밀번호를 입력하세요: ");
    scanf("%s", password);

    for (int i = 0; i < admin_count; i++) {
        if (strcmp(username, admins[i].username) == 0 && strcmp(password, admins[i].password) == 0) {
            printf("관리자 로그인 성공!\n");
            admin_menu();
            return;
        }
    }
    printf("로그인 실패. 관리자 계정을 확인해주세요.\n");
}

// 관리자 메뉴
void admin_menu() {
    int choice;
    while (1) {
        printf("==========================================\n");
        printf("        관리자 메뉴\n");
        printf("==========================================\n");
        printf("[1] 영화 추가\n[2] 영화 수정\n[3] 영화 삭제\n[4] 로그아웃\n");
        printf("번호를 입력하세요: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("잘못된 입력입니다.\n");
            continue;
        }

        if (choice == 1) {
            add_movie();
        }
        else if (choice == 2) {
            modify_movie();
        }
        else if (choice == 3) {
            delete_movie();
        }
        else if (choice == 4) {
            break;
        }
        else {
            printf("잘못된 입력입니다. 다시 시도해주세요.\n");
        }
    }
}

// 영화 추가
void add_movie() {
    char title[50], genre[20], director[50];
    int release_year;

    printf("영화 제목: ");
    scanf("%s", title);
    printf("장르: ");
    scanf("%s", genre);
    printf("개봉 연도: ");
    if (scanf("%d", &release_year) != 1) {
        while (getchar() != '\n');
        printf("잘못된 입력입니다.\n");
        return;
    }
    printf("감독: ");
    scanf("%s", director);

    movies[movie_count].id = movie_count + 1;
    strcpy(movies[movie_count].title, title);
    strcpy(movies[movie_count].genre, genre);
    movies[movie_count].release_year = release_year;
    strcpy(movies[movie_count].director, director);
    movies[movie_count].rating_count = 0;
    movie_count++;

    printf("영화가 추가되었습니다.\n");
    save_movies_to_file(); // 실시간 저장
}

// 영화 수정
void modify_movie() {
    int id;
    printf("수정할 영화의 ID를 입력하세요: ");
    if (scanf("%d", &id) != 1) {
        while (getchar() != '\n');
        printf("잘못된 입력입니다.\n");
        return;
    }

    for (int i = 0; i < movie_count; i++) {
        if (movies[i].id == id) {
            printf("새 영화 제목: ");
            scanf("%s", movies[i].title);
            printf("새 장르: ");
            scanf("%s", movies[i].genre);
            printf("새 개봉 연도: ");
            if (scanf("%d", &movies[i].release_year) != 1) {
                while (getchar() != '\n');
                printf("잘못된 입력입니다.\n");
                return;
            }
            printf("새 감독: ");
            scanf("%s", movies[i].director);

            printf("영화 정보가 수정되었습니다.\n");
            save_movies_to_file(); // 실시간 저장
            return;
        }
    }
    printf("해당 영화 ID를 찾을 수 없습니다.\n");
}

// 영화 삭제
void delete_movie() {
    int id;
    printf("삭제할 영화의 ID를 입력하세요: ");
    if (scanf("%d", &id) != 1) {
        while (getchar() != '\n');
        printf("잘못된 입력입니다.\n");
        return;
    }

    for (int i = 0; i < movie_count; i++) {
        if (movies[i].id == id) {
            for (int j = i; j < movie_count - 1; j++) {
                movies[j] = movies[j + 1];
            }
            movie_count--;
            printf("영화가 삭제되었습니다.\n");
            save_movies_to_file(); // 실시간 저장
            return;
        }
    }
    printf("해당 영화 ID를 찾을 수 없습니다.\n");
}
