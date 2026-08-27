#include <iostream>
#include <queue>
#include <vector>
#include <climits>
#include <fstream>
#include <sstream>
#ifdef WIN32
#include <windows.h>
#endif

#define ERROR_CODE (-1)

using namespace std;

constexpr int MIN_VAL = -150;
constexpr int MAX_VAL = 150;
constexpr int OFFSET = 150;
constexpr int STATE_COUNT = MAX_VAL - MIN_VAL + 1; // 301

constexpr int steps[] = {-3, -6, -9, -15, 2, 7, 13, 16};
constexpr int step_count = sizeof(steps) / sizeof(steps[0]);

struct StateInfo {
    int prev_val;      // 父状态值
    int used_step;     // 从父状态到当前状态所用的步长
    int dist;
    bool reachable;
};

vector<StateInfo> info(STATE_COUNT);

// 预计算
void precompute() {
    for (int i = 0; i < STATE_COUNT; ++i) {
        info[i].reachable = false;
        info[i].dist = INT_MAX;
        info[i].prev_val = 0;
        info[i].used_step = 0;
    }

    queue<int> q;
    info[OFFSET].reachable = true;
    info[OFFSET].dist = 0;
    q.push(0);

    while (!q.empty()) {
        const int cur = q.front();
        q.pop();
        const int cur_idx = cur + OFFSET;

        for (const int s : steps) {
            int nxt = cur + s;
            if (nxt < MIN_VAL || nxt > MAX_VAL)
                continue;
            const int nxt_idx = nxt + OFFSET;
            if (!info[nxt_idx].reachable) {
                info[nxt_idx].reachable = true;
                info[nxt_idx].dist = info[cur_idx].dist + 1;
                info[nxt_idx].prev_val = cur;
                info[nxt_idx].used_step = s;   // 从 cur 到 nxt 所用的步长
                q.push(nxt);
            }
        }
    }
}

/**
 * 获取下一步应执行的操作数。
 * @param diff 当前差值（目标值 - 当前值 - 偏移），范围 [-150, 150]
 * @return 操作数位置（如 1, 6 等）；若 diff==0 返回 -1；若非法或不可达返回 -1（错误标记）
 */
int getNextStep(const int diff) {
    if (diff < MIN_VAL || diff > MAX_VAL) {
        return ERROR_CODE;
    }

    if (diff == 0) {
        return ERROR_CODE;
    }

    // used_step 即为从父状态到当前状态的步长，
    // 执行该步长后，差值变为父状态的值，从而向 0 靠近
    const int step = info[diff + OFFSET].used_step;
    for (int i = 0; i < step_count; ++i) {
        if (step == steps[i]) {
            return i;
        }
    }
    return ERROR_CODE;
}

int main() {
#ifdef WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    precompute();

    ofstream outfile("steps.bin", ios::binary);
    if (!outfile) {
        cerr << "无法创建输出文件 steps.bin" << endl;
        return 1;
    }

    for (int i = MIN_VAL; i <= MAX_VAL; ++i) {
        outfile << static_cast<byte>(getNextStep(i));
    }

    return 0;
}