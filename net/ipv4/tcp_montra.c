
/* Scalable TCP with tunable parameters
 *
 * Bruce Spang <bspang@brucespang.com>
 */

#include <linux/module.h>
#include <net/tcp.h>

/* These factors derived from the recommended values in the aer:
 * .01 and and 7/8. We use 50 instead of 100 to account for
 * delayed ack.
 */
#define TCP_MONTRA_AI_CNT	50U
#define TCP_MONTRA_MD_SCALE	3

struct montra {
	u32 loss_cwnd;
};

static void tcp_montra_cong_avoid(struct sock *sk, u32 ack, u32 acked)
{
	struct tcp_sock *tp = tcp_sk(sk);

	if (!tcp_is_cwnd_limited(sk))
		return;

	if (tcp_in_slow_start(tp))
		tcp_slow_start(tp, acked);
	else
		tcp_cong_avoid_ai(tp, min(tp->snd_cwnd, tp->ai_cnt || TCP_MONTRA_AI_CNT),
				  1);
}

static u32 tcp_montra_ssthresh(struct sock *sk)
{
	const struct tcp_sock *tp = tcp_sk(sk);
	struct montra *ca = inet_csk_ca(sk);

	ca->loss_cwnd = tp->snd_cwnd;

	return max(tp->snd_cwnd - (tp->snd_cwnd>>(tp->md_scale || TCP_MONTRA_MD_SCALE)), 2U);
}

static u32 tcp_montra_cwnd_undo(struct sock *sk)
{
	const struct montra *ca = inet_csk_ca(sk);

	return max(tcp_sk(sk)->snd_cwnd, ca->loss_cwnd);
}

static struct tcp_congestion_ops tcp_montra __read_mostly = {
	.ssthresh	= tcp_montra_ssthresh,
	.undo_cwnd	= tcp_montra_cwnd_undo,
	.cong_avoid	= tcp_montra_cong_avoid,

	.owner		= THIS_MODULE,
	.name		= "montra",
};

static int __init tcp_montra_register(void)
{
	return tcp_register_congestion_control(&tcp_montra);
}

static void __exit tcp_montra_unregister(void)
{
	tcp_unregister_congestion_control(&tcp_montra);
}

module_init(tcp_montra_register);
module_exit(tcp_montra_unregister);

MODULE_AUTHOR("Bruce Spang");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MONTra TCP");
