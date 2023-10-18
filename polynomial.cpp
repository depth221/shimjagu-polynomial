#include <cmath>
#include <cstring>
#include <string>

#include "polynomial.h"

// 원형 리스트 내부에서 last를 제외한 모든 ChainNode를 삭제하는 함수
template <class T>
void ClearCircularList(CircularList<T> &circular_list);

// 원형 리스트 내부의 ChainNode를 전부 삭제하는 함수
template <class T>
void DeleteCircularList(CircularList<T> &circular_list);

Polynomial::Polynomial() = default;

// (a) Read
istream &operator>>(istream &is, Polynomial &x) {
    std::string is_str;
    std::getline(is, is_str);
    std::string input_str;
    char *is_str_c_str = new char[is_str.length() + 1];
    std::memcpy(is_str_c_str, is_str.c_str(), (is_str.length() + 1) * sizeof(char));
    char *input_str_ptr = std::strtok(is_str_c_str, ",");
    if (input_str_ptr == nullptr) return is;

    int terms = std::stoi(input_str_ptr); // 차수
    if (terms == '\n') return is; // 개행 문자가 나올 경우 함수 종료

    // 오름차순으로 정렬된 입력 값을 내림차순으로 정렬하기 위해 임시 배열 생성
    int* numbers = new int[terms * 2];
    
    // is가 빌 때까지 숫자를 쉼표 단위로 읽은 후 저장
    for (int i = 0; i < terms; i++) {
        // 계수
        input_str_ptr = std::strtok(nullptr, ",");
        if (input_str_ptr == nullptr) {
            delete[] numbers;
            return is;
        }
        int coef = std::stoi(input_str_ptr);
        
        // 차수
        input_str_ptr = std::strtok(nullptr, ",");
        if (input_str_ptr == nullptr) {
            delete[] numbers;
            return is;
        }
        int exp = std::stoi(input_str_ptr);

        numbers[(terms * 2 - 1) - 2 * i - 1] = coef;
        numbers[(terms * 2 - 1) - 2 * i] = exp;
    }

    // 임시 배열에 저장된 값을 역순으로 this에 삽입
    for (int i = 0; i < terms; i++) {
        // 계수가 0이면 삽입하지 않음
        if (numbers[2 * i] == 0) continue;
        
        Term tmp_term;
        tmp_term.Set(numbers[2 * i], numbers[2 * i + 1]);
        x.poly.InsertFront(tmp_term);
    }

    delete[] numbers;

    return is;
}

// (b) External representaion
ostream &operator<<(ostream &os, const Polynomial &x) {
    const ChainNode<Term> *head_term = x.poly.GetLast();
    ChainNode<Term> *tmp_term = head_term->GetLink();

    int count = 0;

    // ChainNode에 head밖에 없으면 0을 출력
    if (head_term == tmp_term) {
        os << count;
        return os;
    }

    std::string tmp_output = "";

    // ChainNode에 저장된 값을 임시 문자열에 저장
    while (true) {
        tmp_output += std::to_string(tmp_term->GetData().coef) + "," + std::to_string(tmp_term->GetData().exp);
        count++;

        tmp_term = tmp_term->GetLink();
        if (head_term == tmp_term) break;
        tmp_output += ",";
    }

    // 임시 문자열에 저장된 값을 출력
    os << count << "," << tmp_output;

    return os;
}

// (c) Copy constructor
Polynomial::Polynomial(const Polynomial &a) {
    // head 포인터 값을 저장해 한 바퀴를 돌았는지 확인
    const ChainNode<Term> *last_ptr = a.poly.GetLast();
    CircularList<Term> tmp_circular_list;

    // 한 바퀴를 돌 때까지 Node의 값을 읽은 후 임시 원형 리스트에 역순으로 삽입 
    ChainNode<Term> *curr_ptr = last_ptr->GetLink();
    while (last_ptr != curr_ptr) {
        tmp_circular_list.InsertFront(curr_ptr->GetData());

        curr_ptr = curr_ptr->GetLink();
    }

    // 임시 원형 리스트에서 요소를 순회해 this->poly에 삽입
    const ChainNode<Term> *ins_last_ptr = tmp_circular_list.GetLast();
    ChainNode<Term> *ins_curr_ptr = ins_last_ptr->GetLink();
    while (ins_last_ptr != ins_curr_ptr) {
        poly.InsertFront(ins_curr_ptr->GetData());

        ins_curr_ptr = ins_curr_ptr->GetLink();
    }

    // 임시 원형 리스트 삭제
    DeleteCircularList(tmp_circular_list);
}

// (d) Assign operator
Polynomial &Polynomial::operator=(const Polynomial &a) {
    // 기존 poly 초기화
    DeleteCircularList(poly);
    poly = CircularList<Term>();

    // head 포인터 값을 저장해 한 바퀴를 돌았는지 확인
    const ChainNode<Term> *last_ptr = a.poly.GetLast();
    CircularList<Term> tmp_circular_list;

    // 한 바퀴를 돌 때까지 Node의 값을 읽은 후 임시 원형 리스트에 역순으로 삽입 
    ChainNode<Term> *curr_ptr = last_ptr->GetLink();
    while (last_ptr != curr_ptr) {
        tmp_circular_list.InsertFront(curr_ptr->GetData());

        curr_ptr = curr_ptr->GetLink();
    }

    // 임시 원형 리스트에서 요소를 순회해 this->poly에 삽입
    ChainNode<Term> *ins_last_ptr = tmp_circular_list.GetLast();
    ChainNode<Term> *ins_curr_ptr = ins_last_ptr->GetLink();
    while (ins_last_ptr != ins_curr_ptr) {
        poly.InsertFront(ins_curr_ptr->GetData());

        ins_curr_ptr = ins_curr_ptr->GetLink();
    }

    // 임시 원형 리스트 삭제
    DeleteCircularList(tmp_circular_list);

    return *this;
}

// (e) Destructor
Polynomial::~Polynomial() {
    DeleteCircularList(poly);
}

// (f) Addition
Polynomial Polynomial::operator+(const Polynomial &b) const {
    // head 포인터 값을 저장해 한 바퀴를 돌았는지 확인
    const ChainNode<Term> *this_last_ptr = poly.GetLast();
    const ChainNode<Term> *b_last_ptr = b.poly.GetLast();

    // 한 바퀴를 돌 때까지 Node의 값을 읽은 후 임시 원형 리스트에 역순으로 삽입 
    CircularList<Term> tmp_this;
    ChainNode<Term> *tmp_this_curr_ptr = this_last_ptr->GetLink();
    while (this_last_ptr != tmp_this_curr_ptr) {
        tmp_this.InsertFront(tmp_this_curr_ptr->GetData());

        tmp_this_curr_ptr = tmp_this_curr_ptr->GetLink();
    }

    CircularList<Term> tmp_b;
    ChainNode<Term> *tmp_b_curr_ptr = b_last_ptr->GetLink();
    while (b_last_ptr != tmp_b_curr_ptr) {
        tmp_b.InsertFront(tmp_b_curr_ptr->GetData());

        tmp_b_curr_ptr = tmp_b_curr_ptr->GetLink();
    }

    const ChainNode<Term> *tmp_this_last_ptr = tmp_this.GetLast();
    const ChainNode<Term> *tmp_b_last_ptr = tmp_b.GetLast();
    ChainNode<Term> *this_curr_ptr = tmp_this_last_ptr->GetLink();
    ChainNode<Term> *b_curr_ptr = tmp_b_last_ptr->GetLink();

    Polynomial result;

    // 오름차순으로 결과 Polynomial에 ChainNode를 삽입
    while (tmp_this_last_ptr != this_curr_ptr && tmp_b_last_ptr != b_curr_ptr) {
        if (this_curr_ptr->GetData().exp < b_curr_ptr->GetData().exp) {
            result.poly.InsertFront(this_curr_ptr->GetData());

            this_curr_ptr = this_curr_ptr->GetLink();
        } else if (this_curr_ptr->GetData().exp > b_curr_ptr->GetData().exp) {
            result.poly.InsertFront(b_curr_ptr->GetData());

            b_curr_ptr = b_curr_ptr->GetLink();
        } else {
            // 차수가 같으면 계수를 더한 후 삽입
            Term tmp_term;
            tmp_term.Set(this_curr_ptr->GetData().coef + b_curr_ptr->GetData().coef,
                         this_curr_ptr->GetData().exp);
            
            // 계수가 0이 아닐 경우 삽입, 0일 경우 삽입하지 않음
            if (this_curr_ptr->GetData().coef + b_curr_ptr->GetData().coef != 0) {
                result.poly.InsertFront(tmp_term);
            }
            

            this_curr_ptr = this_curr_ptr->GetLink();
            b_curr_ptr = b_curr_ptr->GetLink();
        }
    }

    // this->poly에 남아 있는 요소를 result에 전부 삽입
    while (tmp_this_last_ptr != this_curr_ptr) {
        result.poly.InsertFront(this_curr_ptr->GetData());
        this_curr_ptr = this_curr_ptr->GetLink();
    }

    // b.poly에 남아 있는 요소를 result에 전부 삽입
    while (tmp_b_last_ptr != b_curr_ptr) {
        result.poly.InsertFront(b_curr_ptr->GetData());
        b_curr_ptr = b_curr_ptr->GetLink();
    }

    // 임시 원형 리스트 삭제
    DeleteCircularList(tmp_this);
    DeleteCircularList(tmp_b);

    return result;
}

// (g) Substarction
Polynomial Polynomial::operator-(const Polynomial &b) const {
    // head 포인터 값을 저장해 한 바퀴를 돌았는지 확인
    const ChainNode<Term> *this_last_ptr = poly.GetLast();
    const ChainNode<Term> *b_last_ptr = b.poly.GetLast();

    // 한 바퀴를 돌 때까지 Node의 값을 읽은 후 임시 원형 리스트에 역순으로 삽입 
    CircularList<Term> tmp_this;
    ChainNode<Term> *tmp_this_curr_ptr = this_last_ptr->GetLink();
    while (this_last_ptr != tmp_this_curr_ptr) {
        tmp_this.InsertFront(tmp_this_curr_ptr->GetData());

        tmp_this_curr_ptr = tmp_this_curr_ptr->GetLink();
    }

    CircularList<Term> tmp_b;
    ChainNode<Term> *tmp_b_curr_ptr = b_last_ptr->GetLink();
    while (b_last_ptr != tmp_b_curr_ptr) {
        tmp_b.InsertFront(tmp_b_curr_ptr->GetData());

        tmp_b_curr_ptr = tmp_b_curr_ptr->GetLink();
    }

    const ChainNode<Term> *tmp_this_last_ptr = tmp_this.GetLast();
    const ChainNode<Term> *tmp_b_last_ptr = tmp_b.GetLast();
    ChainNode<Term> *this_curr_ptr = tmp_this_last_ptr->GetLink();
    ChainNode<Term> *b_curr_ptr = tmp_b_last_ptr->GetLink();

    Polynomial result;

    // 오름차순으로 결과 Polynomial에 ChainNode를 삽입
    while (tmp_this_last_ptr != this_curr_ptr && tmp_b_last_ptr != b_curr_ptr) {
        if (this_curr_ptr->GetData().exp < b_curr_ptr->GetData().exp) {
            result.poly.InsertFront(this_curr_ptr->GetData());

            this_curr_ptr = this_curr_ptr->GetLink();
        } else if (this_curr_ptr->GetData().exp > b_curr_ptr->GetData().exp) {
            result.poly.InsertFront(b_curr_ptr->GetData());

            b_curr_ptr = b_curr_ptr->GetLink();
        } else {
            // 차수가 같으면 계수를 뺀 후 삽입
            Term tmp_term;
            tmp_term.Set(this_curr_ptr->GetData().coef - b_curr_ptr->GetData().coef,
                         this_curr_ptr->GetData().exp);
            
            // 계수가 0이 아닐 경우 삽입, 0일 경우 삽입하지 않음
            if (this_curr_ptr->GetData().coef - b_curr_ptr->GetData().coef != 0) {
                result.poly.InsertFront(tmp_term);
            }

            this_curr_ptr = this_curr_ptr->GetLink();
            b_curr_ptr = b_curr_ptr->GetLink();
        }
    }

    // this->poly에 남아 있는 요소를 result에 전부 삽입
    while (tmp_this_last_ptr != this_curr_ptr) {
        result.poly.InsertFront(this_curr_ptr->GetData());
        this_curr_ptr = this_curr_ptr->GetLink();
    }

    // b.poly에 남아 있는 요소를 result에 전부 삽입
    while (tmp_b_last_ptr != b_curr_ptr) {
        result.poly.InsertFront(b_curr_ptr->GetData());
        b_curr_ptr = b_curr_ptr->GetLink();
    }

    // 임시 원형 리스트 삭제
    DeleteCircularList(tmp_this);
    DeleteCircularList(tmp_b);

    return result;
}

// (h) Multiplication
Polynomial Polynomial::operator*(const Polynomial &b) const {
    int max_exp = poly.GetLast()->GetLink()->GetData().exp + b.poly.GetLast()->GetLink()->GetData().exp;
    // 곱셈 결과를 임시적으로 저장하는 배열
    int* numbers = new int[max_exp + 1];
    std::memset(numbers, 0, sizeof(int) * (max_exp + 1));

    const ChainNode<Term> *this_last_ptr = poly.GetLast();
    const ChainNode<Term> *b_last_ptr = b.poly.GetLast();
    ChainNode<Term> *this_curr_ptr = this_last_ptr->GetLink();
    ChainNode<Term> *b_curr_ptr = b_last_ptr->GetLink();

    Polynomial result;

    // 각 항을 곱한 뒤 임시 배열에 결과를 저장
    while (this_last_ptr != this_curr_ptr) {
        b_curr_ptr = b_last_ptr->GetLink();

        while (b_last_ptr != b_curr_ptr) {
            numbers[this_curr_ptr->GetData().exp + b_curr_ptr->GetData().exp] += this_curr_ptr->GetData().coef * b_curr_ptr->GetData().coef;

            b_curr_ptr = b_curr_ptr->GetLink();
        }

        this_curr_ptr = this_curr_ptr->GetLink();
    }

    // 임시 배열의 결과를 result에 삽입
    for (int i = 0; i < max_exp + 1; i++) {
        if (numbers[i] == 0) continue;

        Term tmp_term;
        tmp_term.Set(numbers[i], i);
        result.poly.InsertFront(tmp_term);
    }

    delete[] numbers;

    return result;
}

// (i) Evaluate polynomial
float Polynomial::Evaluate(float x) const {
    float result = 0.0;

    const ChainNode<Term> *this_last_ptr = poly.GetLast();
    ChainNode<Term> *this_curr_ptr = this_last_ptr->GetLink();

    while (this_last_ptr != this_curr_ptr) {
        result += this_curr_ptr->GetData().coef * std::pow(x, this_curr_ptr->GetData().exp);

        this_curr_ptr = this_curr_ptr->GetLink();
    }

    return result;
}

// 원형 리스트 내부에서 last를 제외한 모든 ChainNode를 삭제하는 함수
template <class T>
void ClearCircularList(CircularList<T> &circular_list) {
    const ChainNode<Term> *last_ptr = circular_list.GetLast();
    ChainNode<Term> *curr_ptr = last_ptr->GetLink();
    ChainNode<Term> *next_ptr = curr_ptr->GetLink();

    while (curr_ptr != last_ptr) {
        next_ptr = curr_ptr->GetLink();
        delete curr_ptr;
        curr_ptr = next_ptr;
    }
}

// 원형 리스트 내부의 ChainNode를 전부 삭제하는 함수
template <class T>
void DeleteCircularList(CircularList<T> &circular_list) {
    ClearCircularList(circular_list);
    delete circular_list.GetLast();
}